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
 * \brief Definition of Address class.
 */
#include "msgpack_rpc/addresses/address.h"

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/addresses/tcp_address.h"

TEST_CASE("msgpack_rpc::addresses::Address") {
    using msgpack_rpc::addresses::Address;
    using msgpack_rpc::addresses::TCPAddress;

    SECTION("create TCP address") {
        const auto address = Address(TCPAddress("1.2.3.4", 12345));

        CHECK(fmt::format("{}", address) == "tcp://1.2.3.4:12345");
        CHECK(fmt::format("{}", address.to_uri()) == "tcp://1.2.3.4:12345");
        CHECK(fmt::format("{}", address.as_tcp()) == "tcp://1.2.3.4:12345");
    }

    SECTION("compare addresses") {
        const auto tcp_address1 = Address(TCPAddress("11.22.33.44", 1234));
        const auto tcp_address2 = Address(TCPAddress("11.22.33.44", 1235));

        CHECK(tcp_address1 == tcp_address1);
        CHECK(tcp_address1 != tcp_address2);
    }
}
