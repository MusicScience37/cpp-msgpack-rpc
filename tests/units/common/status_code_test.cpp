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
 * \brief Test of functions for status codes.
 */
#include "msgpack_rpc/common/status_code.h"

#include <string>
#include <string_view>
#include <unordered_map>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("using msgpack_rpc::format_status_code") {
    using msgpack_rpc::format_status_code;
    using msgpack_rpc::StatusCode;

    SECTION("get formatted string") {
        const auto result_map =
            std::unordered_map<StatusCode, std::string_view>{
                {StatusCode::SUCCESS, "SUCCESS"},
                {StatusCode::INVALID_ARGUMENT, "INVALID_ARGUMENT"},
                {StatusCode::INVALID_MESSAGE, "INVALID_MESSAGE"},
                {StatusCode::TYPE_ERROR, "TYPE_ERROR"},
                {StatusCode::PRECONDITION_NOT_MET, "PRECONDITION_NOT_MET"},
                {StatusCode::OPERATION_ABORTED, "OPERATION_ABORTED"},
                {StatusCode::HOST_UNRESOLVED, "HOST_UNRESOLVED"},
                {StatusCode::CONNECTION_FAILURE, "CONNECTION_FAILURE"},
                {StatusCode::TIMEOUT, "TIMEOUT"},
                {StatusCode::SERVER_ERROR, "SERVER_ERROR"},
                {StatusCode::UNEXPECTED_ERROR, "UNEXPECTED_ERROR"},
                {static_cast<StatusCode>(
                     static_cast<int>(StatusCode::UNEXPECTED_ERROR) + 1),
                    "INVALID_STATUS_CODE"}};

        for (const auto& [value, string] : result_map) {
            INFO("string: " << string);
            CHECK(format_status_code(value) == string);
        }
    }
}
