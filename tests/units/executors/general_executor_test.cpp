/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Test of GeneralExecutor class.
 */
#include <atomic>
#include <chrono>
#include <exception>
#include <future>
#include <stdexcept>
#include <thread>

#include <asio/post.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include "../create_test_logger.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"

TEST_CASE("msgpack_rpc::executors::GeneralExecutor") {
    using msgpack_rpc::config::ExecutorConfig;
    using msgpack_rpc::executors::create_executor;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc_test::create_test_logger;

    const auto logger = create_test_logger();
    const auto executor_config = ExecutorConfig();
    const auto executor = create_executor(logger, executor_config);

    SECTION("run with a task") {
        std::atomic<bool> is_called{false};
        const OperationType operation_type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        INFO("Operation type: " << static_cast<int>(operation_type));
        MSGPACK_RPC_DEBUG(
            logger, "Operation type: {}", static_cast<int>(operation_type));
        CHECK_NOTHROW(asio::post(
            executor->context(operation_type), [&is_called, &executor] {
                is_called.store(true);
                executor->interrupt();
            }));

        CHECK_NOTHROW(executor->run());

        CHECK(is_called.load());
    }

    SECTION("run with a task throwing an exception") {
        std::atomic<bool> is_called{false};
        const std::string message = "Test exception message.";
        const OperationType operation_type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        INFO("Operation type: " << static_cast<int>(operation_type));
        MSGPACK_RPC_DEBUG(
            logger, "Operation type: {}", static_cast<int>(operation_type));
        CHECK_NOTHROW(asio::post(
            executor->context(operation_type), [&is_called, &message] {
                is_called.store(true);
                throw std::runtime_error(message);
            }));

        CHECK_THROWS_WITH(executor->run(), message);

        CHECK(is_called.load());
    }

    SECTION("start and stop") {
        CHECK_NOTHROW(executor->start());

        std::promise<void> called_promise;
        auto future = called_promise.get_future();
        const OperationType operation_type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        INFO("Operation type: " << static_cast<int>(operation_type));
        MSGPACK_RPC_DEBUG(
            logger, "Operation type: {}", static_cast<int>(operation_type));
        CHECK_NOTHROW(asio::post(executor->context(operation_type),
            [&called_promise] { called_promise.set_value(); }));

        CHECK(future.wait_for(std::chrono::seconds(1)) ==
            std::future_status::ready);

        CHECK_NOTHROW(executor->stop());
    }

    SECTION("get the last exception in threads") {
        CHECK_NOTHROW(executor->start());

        const std::string message = "Test exception message.";
        const OperationType operation_type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        INFO("Operation type: " << static_cast<int>(operation_type));
        MSGPACK_RPC_DEBUG(
            logger, "Operation type: {}", static_cast<int>(operation_type));
        CHECK_NOTHROW(asio::post(executor->context(operation_type),
            [&message] { throw std::runtime_error(message); }));

        std::exception_ptr last_exception;
        const auto deadline =
            std::chrono::steady_clock::now() + std::chrono::seconds(1);
        while (!last_exception && std::chrono::steady_clock::now() < deadline) {
            CHECK_NOTHROW(last_exception = executor->last_exception());
            std::this_thread::sleep_for(
                std::chrono::milliseconds(10));  // NOLINT
        }
        REQUIRE(last_exception);
        CHECK_THROWS_WITH(std::rethrow_exception(last_exception), message);

        CHECK_NOTHROW(executor->stop());
    }
}
