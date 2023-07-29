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
 * \brief Test of ServerConfig class.
 */
#include "msgpack_rpc/config/server_config.h"

#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/addresses/uri.h"

TEST_CASE("msgpack_rpc::config::ServerConfig") {
    using msgpack_rpc::addresses::TCP_SCHEME;
    using msgpack_rpc::config::ServerConfig;

    SECTION("add an TCP endpoint") {
        ServerConfig config;

        const auto host = std::string_view("localhost");
        const auto port = static_cast<std::uint16_t>(12345);
        const auto& endpoint_config = config.add_tcp_endpoint(host, port);

        CHECK(endpoint_config.uri().scheme() == TCP_SCHEME);
        CHECK(endpoint_config.uri().host() == host);
        CHECK(endpoint_config.uri().port_number() == port);

        SECTION("and get the endpoints") {
            const auto& endpoints = config.endpoints();

            CHECK(endpoints.size() == 1);
            CHECK(endpoints.at(0).uri().scheme() == TCP_SCHEME);
            CHECK(endpoints.at(0).uri().host() == host);
            CHECK(endpoints.at(0).uri().port_number() == port);
        }
    }
}
