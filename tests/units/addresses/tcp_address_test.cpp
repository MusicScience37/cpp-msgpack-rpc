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
 * \brief Test of TCPAddress.
 */
#include "msgpack_rpc/addresses/tcp_address.h"

#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "approval_test_fmt.h"

TEST_CASE("msgpack_rpc::addresses::TCPAddress") {
    using msgpack_rpc::addresses::TCPAddress;

    SECTION("create an IP v4 address") {
        const auto ip_address = std::string_view("11.22.33.44");
        const auto port_number = static_cast<std::uint16_t>(1234);

        const auto address = TCPAddress(ip_address, port_number);

        ApprovalTests::Approvals::verify(fmt::format(R"(IP: {}
Port: {}
Full: {}
URI: {})",
            address.ip_address(), address.port_number(), address,
            address.to_uri()));
    }

    SECTION("create an IP v6 address") {
        const auto ip_address = std::string_view("fc00::1");
        const auto port_number = static_cast<std::uint16_t>(12345);

        const auto address = TCPAddress(ip_address, port_number);

        ApprovalTests::Approvals::verify(fmt::format(R"(IP: {}
Port: {}
Full: {}
URI: {})",
            address.ip_address(), address.port_number(), address,
            address.to_uri()));
    }

    SECTION("compare addresses") {
        const auto address1 = TCPAddress("11.22.33.44", 1234);
        const auto address2 = TCPAddress("11.22.33.44", 1235);
        const auto address3 = TCPAddress("11.22.33.45", 1234);

        CHECK(address1 == address1);
        CHECK(address1 != address2);
        CHECK(address1 != address3);
    }
}
