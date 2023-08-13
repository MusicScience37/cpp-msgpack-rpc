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
 * \brief Test of ClientConfig class.
 */
#include "msgpack_rpc/config/client_config.h"

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/addresses/uri.h"

TEST_CASE("msgpack_rpc::config::ClientConfig") {
    using msgpack_rpc::addresses::TCP_SCHEME;
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::ClientConfig;

    SECTION("add a URI") {
        ClientConfig config;

        config.add_uri("tcp://localhost:12345");

        CHECK(config.uris() ==
            std::vector<URI>{URI::parse("tcp://localhost:12345")});
    }

    SECTION("get the configuration of parsers of messages") {
        ClientConfig config;

        CHECK_NOTHROW((void)config.message_parser());
        CHECK_NOTHROW(
            (void)static_cast<const ClientConfig&>(config).message_parser());
    }

    SECTION("get the configuration of executors") {
        ClientConfig config;

        CHECK_NOTHROW((void)config.executor());
        CHECK_NOTHROW(
            (void)static_cast<const ClientConfig&>(config).executor());
    }
}
