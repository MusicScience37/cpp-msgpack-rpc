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
 * \brief Test of Status class.
 */
#include "msgpack_rpc/common/status.h"

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/common/status_code.h"

TEST_CASE("msgpack_rpc::Status") {
    using msgpack_rpc::Status;
    using msgpack_rpc::StatusCode;

    SECTION("create using default constructor") {
        const auto status = Status();

        CHECK(status.code() == StatusCode::SUCCESS);
        CHECK(status.message() == std::string_view());  // NOLINT
    }

    SECTION("create success using constructor") {
        const auto status = Status(StatusCode::SUCCESS, "Ignored message.");

        CHECK(status.code() == StatusCode::SUCCESS);
        CHECK(status.message() == std::string_view());  // NOLINT
    }

    SECTION("create an error using constructor") {
        const auto status =
            Status(StatusCode::INVALID_ARGUMENT, "Test message.");

        CHECK(status.code() == StatusCode::INVALID_ARGUMENT);
        CHECK(status.message() == std::string_view("Test message."));
    }
}
