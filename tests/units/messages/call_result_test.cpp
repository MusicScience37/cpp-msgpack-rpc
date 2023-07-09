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
 * \brief Definition of CallResult class.
 */
#include "msgpack_rpc/messages/call_result.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

TEST_CASE("msgpack_rpc::messages::CallResult") {
    using msgpack_rpc::messages::CallResult;

    SECTION("create a result") {
        const auto zone = std::make_shared<msgpack::zone>();
        const auto object = msgpack::object("abc", *zone);

        const auto result = CallResult::create_result(object, zone);

        CHECK_FALSE(result.is_error());
        CHECK(result.is_success());
        CHECK(result.result_as<std::string>() == "abc");
        CHECK_THROWS(result.error_as<std::string>());
    }

    SECTION("create an error") {
        const auto zone = std::make_shared<msgpack::zone>();
        const auto object = msgpack::object("abc", *zone);

        const auto result = CallResult::create_error(object, zone);

        CHECK(result.is_error());
        CHECK_FALSE(result.is_success());
        CHECK(result.error_as<std::string>() == "abc");
        CHECK_THROWS(result.result_as<std::string>());
    }

    SECTION("try to get a result as a wrong type") {
        const auto zone = std::make_shared<msgpack::zone>();
        const auto object = msgpack::object("abc", *zone);
        const auto result = CallResult::create_result(object, zone);

        CHECK_THROWS(result.result_as<int>());
    }

    SECTION("try to get an error as a wrong type") {
        const auto zone = std::make_shared<msgpack::zone>();
        const auto object = msgpack::object("abc", *zone);
        const auto result = CallResult::create_error(object, zone);

        CHECK_THROWS(result.error_as<int>());
    }
}
