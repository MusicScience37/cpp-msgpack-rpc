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
 * \brief Test of Timer class.
 */
#include "msgpack_rpc/executors/timer.h"

#include <chrono>
#include <functional>
#include <ratio>

#include <catch2/catch_test_macros.hpp>

#include "../create_test_logger.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/operation_type.h"

TEST_CASE("msgpack_rpc::executors::Timer") {
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::executors::Timer;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto post = [&executor](std::function<void()> function) {
        msgpack_rpc::executors::async_invoke(
            executor, OperationType::CALLBACK, std::move(function));
    };

    Timer timer(executor, OperationType::CALLBACK);

    SECTION("wait until given time") {
        int num_calls = 0;
        const auto function = [&num_calls] { ++num_calls; };

        post([&timer, &function] {
            static constexpr auto duration = std::chrono::milliseconds(100);
            timer.async_sleep_until(
                std::chrono::steady_clock::now() + duration, function);
        });
        executor->run();

        CHECK(num_calls == 1);
    }

    SECTION("wait until given time before call") {
        int num_calls = 0;
        const auto function = [&num_calls] { ++num_calls; };

        post([&timer, &function] {
            static constexpr auto duration = std::chrono::milliseconds(-100);
            timer.async_sleep_until(
                std::chrono::steady_clock::now() + duration, function);
        });
        executor->run();

        CHECK(num_calls == 1);
    }

    SECTION("wait for given time") {
        int num_calls = 0;
        const auto function = [&num_calls] { ++num_calls; };

        post([&timer, &function] {
            static constexpr auto duration = std::chrono::milliseconds(100);
            timer.async_sleep_for(duration, function);
        });
        executor->run();

        CHECK(num_calls == 1);
    }

    SECTION("cancel waiting") {
        int num_calls = 0;
        const auto function = [&num_calls] { ++num_calls; };

        post([&timer, &function] {
            static constexpr auto duration = std::chrono::seconds(1);
            timer.async_sleep_for(duration, function);
            timer.cancel();
        });
        executor->run();

        CHECK(num_calls == 0);
    }
}
