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
#include <chrono>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "../create_test_logger.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "wait_last_exception_in.h"

TEST_CASE("msgpack_rpc::executors::GeneralExecutor") {
    using msgpack_rpc::config::ExecutorConfig;
    using msgpack_rpc::executors::async_invoke;
    using msgpack_rpc::executors::create_executor;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc_test::create_test_logger;

    const auto logger = create_test_logger();
    const auto executor_config = ExecutorConfig();
    const auto executor = create_executor(logger, executor_config);

    SECTION("start and stop") {
        CHECK_FALSE(executor->is_running());
        CHECK_NOTHROW(executor->start());
        CHECK(executor->is_running());

        std::promise<void> called_promise;
        auto future = called_promise.get_future();
        const OperationType operation_type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        INFO("Operation type: " << static_cast<int>(operation_type));
        MSGPACK_RPC_DEBUG(
            logger, "Operation type: {}", static_cast<int>(operation_type));
        CHECK_NOTHROW(async_invoke(executor, operation_type,
            [&called_promise] { called_promise.set_value(); }));

        CHECK(future.wait_for(std::chrono::seconds(1)) ==
            std::future_status::ready);

        CHECK(executor->is_running());
        CHECK_NOTHROW(executor->stop());
        CHECK_FALSE(executor->is_running());
    }

    SECTION("get the last exception in threads") {
        CHECK_FALSE(executor->is_running());
        CHECK_NOTHROW(executor->start());
        CHECK(executor->is_running());

        const std::string message = "Test exception message.";
        const OperationType operation_type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        INFO("Operation type: " << static_cast<int>(operation_type));
        MSGPACK_RPC_DEBUG(
            logger, "Operation type: {}", static_cast<int>(operation_type));
        CHECK_NOTHROW(async_invoke(executor, operation_type,
            [&message] { throw std::runtime_error(message); }));

        std::exception_ptr last_exception = wait_last_exception_in(executor);
        CHECK_THROWS_WITH(std::rethrow_exception(last_exception), message);

        CHECK_FALSE(executor->is_running());
        CHECK_NOTHROW(executor->stop());
        CHECK_FALSE(executor->is_running());
    }

    SECTION("set a callback for exceptions in threads") {
        CHECK_FALSE(executor->is_running());
        CHECK_NOTHROW(executor->start());
        CHECK(executor->is_running());

        std::vector<std::exception_ptr> exceptions;
        executor->on_exception(
            [&exceptions](const std::exception_ptr& exception) {
                exceptions.push_back(exception);
            });

        const std::string message = "Test exception message.";
        const OperationType operation_type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        INFO("Operation type: " << static_cast<int>(operation_type));
        MSGPACK_RPC_DEBUG(
            logger, "Operation type: {}", static_cast<int>(operation_type));
        CHECK_NOTHROW(async_invoke(executor, operation_type,
            [&message] { throw std::runtime_error(message); }));

        wait_last_exception_in(executor);

        CHECK_FALSE(executor->is_running());
        CHECK_NOTHROW(executor->stop());
        CHECK_FALSE(executor->is_running());

        CHECK(exceptions.size() == static_cast<std::size_t>(1));
        CHECK_THROWS_WITH(std::rethrow_exception(exceptions.at(0)), message);
    }
}
