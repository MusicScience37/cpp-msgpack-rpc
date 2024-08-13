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
#include <fmt/format.h>

#include "msgpack_rpc/impl/config.h"

TEST_CASE("msgpack_rpc::addresses::URI") {
    using msgpack_rpc::addresses::URI;

    SECTION("parse a URI of TCP") {
        const URI uri = URI::parse("tcp://example.com:12345");

        CHECK(uri.scheme() == "tcp");
        CHECK(uri.host_or_filepath() == "example.com");
        CHECK(uri.port_number() == static_cast<std::uint16_t>(12345));
        CHECK(fmt::format("{}", uri) == "tcp://example.com:12345");
    }

    SECTION("parse a URI of IP v6") {
        const URI uri = URI::parse("tcp://[fc00::3]:65535");

        CHECK(uri.scheme() == "tcp");
        CHECK(uri.host_or_filepath() == "fc00::3");
        CHECK(uri.port_number() == static_cast<std::uint16_t>(65535));
        CHECK(fmt::format("{}", uri) == "tcp://[fc00::3]:65535");
    }

#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS

    SECTION("parse a URI of Unix socket") {
        const URI uri = URI::parse("unix:///test/path");

        CHECK(uri.scheme() == "unix");
        CHECK(uri.host_or_filepath() == "/test/path");
        CHECK(uri.port_number() == std::nullopt);
        CHECK(fmt::format("{}", uri) == "unix:///test/path");
    }

#endif

    SECTION("parse invalid URIs") {
        CHECK_THROWS((void)URI::parse("tcp://example.com:65536"));
        CHECK_THROWS((void)URI::parse("tcp://[fc00::3]:65536"));
        CHECK_THROWS((void)URI::parse("invalid://example.com:65535"));
        CHECK_THROWS((void)URI::parse("invalid://example/path"));
        CHECK_THROWS((void)URI::parse("abc"));
    }

    SECTION("compare addresses") {
        const auto address1 = URI::parse("tcp://11.22.33.44:1234");
        const auto address2 = URI::parse("tcp://11.22.33.44:1235");
        const auto address3 = URI::parse("tcp://11.22.33.45:1234");

        CHECK(address1 == address1);
        CHECK(address1 != address2);
        CHECK(address1 != address3);
    }
}
