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
 * \brief Definition of wait_last_exception_in function.
 */
#pragma once

#include <chrono>
#include <exception>
#include <memory>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/executors/i_async_executor.h"

std::exception_ptr wait_last_exception_in(
    const std::shared_ptr<msgpack_rpc::executors::IAsyncExecutor>& executor) {
    std::exception_ptr last_exception;
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (!last_exception && std::chrono::steady_clock::now() < deadline) {
        CHECK_NOTHROW(last_exception = executor->last_exception());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // NOLINT
    }
    REQUIRE(last_exception);
    return last_exception;
}
