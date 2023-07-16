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
 * \brief Test of URI class.
 */
#include "msgpack_rpc/addresses/uri.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::addresses::URI") {
    using msgpack_rpc::addresses::URI;

    SECTION("parse a URI of TCP") {
        const URI uri = URI::parse("tcp://example.com:12345");

        CHECK(uri.schema() == "tcp");
        CHECK(uri.host() == "example.com");
        CHECK(uri.port_number() == static_cast<std::uint16_t>(12345));
    }

    SECTION("parse a URI of IP v6") {
        const URI uri = URI::parse("tcp://[fc00::3]:65535");

        CHECK(uri.schema() == "tcp");
        CHECK(uri.host() == "fc00::3");
        CHECK(uri.port_number() == static_cast<std::uint16_t>(65535));
    }

    SECTION("parse a URI without port number") {
        const URI uri = URI::parse("unix://test.socket");

        CHECK(uri.schema() == "unix");
        CHECK(uri.host() == "test.socket");
    }

    SECTION("parse invalid URIs") {
        CHECK_THROWS((void)URI::parse("tcp://example.com:65536"));
        CHECK_THROWS((void)URI::parse("tcp://[fc00::3]:65536"));
        CHECK_THROWS((void)URI::parse("abc"));
    }
}
