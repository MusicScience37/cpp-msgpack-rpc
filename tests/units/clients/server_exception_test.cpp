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
 * \brief Test of ServerException class.
 */
#include "msgpack_rpc/clients/server_exception.h"

#include <memory>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::clients::ServerException") {
    using msgpack_rpc::clients::ServerException;

    SECTION("create an exception") {
        const auto zone = std::make_shared<msgpack::zone>();
        const auto object = msgpack::object("test message", *zone);

        const auto exception = ServerException(object, zone);

        SECTION("and used as an exception") {
            const auto function = [&exception] {
                throw ServerException(exception);
            };

            CHECK_THROWS_AS(function(), ServerException);
        }

        SECTION("and get the object") {
            CHECK(exception.object().as<std::string_view>() == "test message");
        }

        SECTION("and get as an object") {
            CHECK(exception.error_as<std::string_view>() == "test message");
        }
    }
}
