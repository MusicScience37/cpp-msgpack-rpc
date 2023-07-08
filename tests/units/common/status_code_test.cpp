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

#include <string_view>
#include <unordered_map>

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
                {static_cast<StatusCode>(
                     static_cast<int>(StatusCode::INVALID_MESSAGE) + 1),
                    "INVALID_STATUS_CODE"}};
    }
}
