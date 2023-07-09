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
 * \brief Test of ParsedParameters class.
 */
#include "msgpack_rpc/messages/parsed_parameters.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::messages::ParsedParameters") {
    using msgpack_rpc::messages::ParsedParameters;

    SECTION("convert parameters") {
        auto zone = std::make_unique<msgpack::zone>();
        const int param1 = 123;
        const std::string param2 = "abc";
        const auto object =
            msgpack::object(std::make_tuple(param1, param2), *zone);
        const auto wrapper = ParsedParameters(object, std::move(zone));

        const std::tuple<int, std::string> result =
            wrapper.as<int, std::string>();

        CHECK(std::get<0>(result) == param1);
        CHECK(std::get<1>(result) == param2);
    }

    SECTION("create from non-array object") {
        auto zone = std::make_unique<msgpack::zone>();
        const auto object = msgpack::object(123, *zone);

        CHECK_THROWS((void)ParsedParameters(object, std::move(zone)));
    }

    SECTION("convert parameters with wrong number of parameters") {
        auto zone = std::make_unique<msgpack::zone>();
        const int param1 = 123;
        const std::string param2 = "abc";
        const auto object =
            msgpack::object(std::make_tuple(param1, param2), *zone);
        const auto wrapper = ParsedParameters(object, std::move(zone));

        CHECK_THROWS((void)wrapper.as<int>());
        CHECK_THROWS((void)wrapper.as<int, std::string, float>());
    }

    SECTION("convert parameters with wrong type") {
        auto zone = std::make_unique<msgpack::zone>();
        const int param1 = 123;
        const std::string param2 = "abc";
        const auto object =
            msgpack::object(std::make_tuple(param1, param2), *zone);
        const auto wrapper = ParsedParameters(object, std::move(zone));

        CHECK_THROWS((void)wrapper.as<int, float>());
    }
}
