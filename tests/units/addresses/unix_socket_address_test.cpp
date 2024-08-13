/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of UnixSocketAddress class.
 */
#include "msgpack_rpc/addresses/unix_socket_address.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_UNIX_SOCKETS

#include <optional>
#include <sstream>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "msgpack_rpc/addresses/uri.h"

TEST_CASE("msgpack_rpc::addresses::UnixSocketAddress") {
    using msgpack_rpc::addresses::UnixSocketAddress;
    using msgpack_rpc::addresses::URI;

    SECTION("get the file path") {
        const UnixSocketAddress address{"file/path.sock"};

        CHECK(address.file_path() == "file/path.sock");
    }

    SECTION("get the URI") {
        const UnixSocketAddress address{"file/path.sock"};

        const URI uri = address.to_uri();

        CHECK(uri.scheme() == "unix");
        CHECK(uri.host_or_file_path() == "file/path.sock");
        CHECK(uri.port_number() == std::nullopt);
    }

    SECTION("get the string expression") {
        const UnixSocketAddress address{"file/path.sock"};

        CHECK(address.to_string() == "unix://file/path.sock");
    }

    SECTION("get the address in asio library") {
        const UnixSocketAddress address{"file/path.sock"};

        CHECK(address.asio_address().path() == "file/path.sock");
    }

    SECTION("compare with other addresses") {
        const UnixSocketAddress address1{"file/path1.sock"};
        const UnixSocketAddress address2{
            "file/path1.sock"};  // same as address1
        const UnixSocketAddress address3{"file/path3.sock"};

        CHECK(address1 == address2);
        CHECK_FALSE(address1 == address3);
        CHECK_FALSE(address1 != address2);
        CHECK(address1 != address3);
    }

    SECTION("format using fmt library") {
        const UnixSocketAddress address{"file/path.sock"};

        CHECK(fmt::format("{}", address) == "unix://file/path.sock");
    }

    SECTION("format using ostream") {
        const UnixSocketAddress address{"file/path.sock"};

        std::ostringstream stream;
        stream << address;
        CHECK(stream.str() == "unix://file/path.sock");
    }
}

#endif
