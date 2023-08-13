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
 * \brief Test of SingleThreadExecutor class.
 */
#include <atomic>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

#include "../create_test_logger.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/operation_type.h"

TEST_CASE("msgpack_rpc::executors::SingleThreadExecutor") {
    using msgpack_rpc::executors::async_invoke;
    using msgpack_rpc::executors::create_single_thread_executor;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc_test::create_test_logger;

    const auto logger = create_test_logger();
    const auto executor = create_single_thread_executor(logger);

    SECTION("run without a task") {
        // This will return soon.
        CHECK_NOTHROW(executor->run());
    }

    SECTION("run with a task") {
        std::atomic<bool> is_called{false};
        CHECK_NOTHROW(async_invoke(executor, OperationType::CALLBACK,
            [&is_called] { is_called.store(true); }));

        CHECK_NOTHROW(executor->run());

        CHECK(is_called.load());
    }

    SECTION("run with a task throwing an exception") {
        std::atomic<bool> is_called1{false};
        const std::string message = "Test exception message.";
        CHECK_NOTHROW(async_invoke(
            executor, OperationType::CALLBACK, [&is_called1, &message] {
                is_called1.store(true);
                throw std::runtime_error(message);
            }));
        std::atomic<bool> is_called2{false};
        CHECK_NOTHROW(async_invoke(executor, OperationType::CALLBACK,
            [&is_called2] { is_called2.store(true); }));

        CHECK_THROWS_WITH(executor->run(), message);

        CHECK(is_called1.load());
        CHECK_FALSE(is_called2.load());
    }

    SECTION("run with a task stopping the executor") {
        std::atomic<bool> is_called1{false};
        CHECK_NOTHROW(async_invoke(
            executor, OperationType::CALLBACK, [&is_called1, &executor] {
                is_called1.store(true);
                executor->interrupt();
            }));
        std::atomic<bool> is_called2{false};
        CHECK_NOTHROW(async_invoke(executor, OperationType::CALLBACK,
            [&is_called2] { is_called2.store(true); }));

        CHECK_NOTHROW(executor->run());

        CHECK(is_called1.load());
        CHECK_FALSE(is_called2.load());
    }
}
