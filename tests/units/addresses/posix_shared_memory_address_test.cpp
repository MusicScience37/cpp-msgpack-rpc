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
 * \brief Test of PosixSharedMemoryAddress class.
 */
#include "msgpack_rpc/addresses/posix_shared_memory_address.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_SHM

#include <optional>
#include <sstream>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "msgpack_rpc/addresses/uri.h"

TEST_CASE("msgpack_rpc::addresses::PosixSharedMemoryAddress") {
    using msgpack_rpc::addresses::PosixSharedMemoryAddress;
    using msgpack_rpc::addresses::URI;

    SECTION("check file names") {
        CHECK_NOTHROW((void)PosixSharedMemoryAddress("file_name"));
        CHECK_THROWS((void)PosixSharedMemoryAddress("file/path"));
        CHECK_THROWS((void)PosixSharedMemoryAddress("/file_name"));
    }

    SECTION("get the file name") {
        const PosixSharedMemoryAddress address{"file_name"};

        CHECK(address.file_name() == "file_name");
    }

    SECTION("get the URI") {
        const PosixSharedMemoryAddress address{"file_name"};

        const URI uri = address.to_uri();

        CHECK(uri.scheme() == "shm");
        CHECK(uri.host_or_file_path() == "file_name");
        CHECK(uri.port_number() == std::nullopt);
    }

    SECTION("get the string expression") {
        const PosixSharedMemoryAddress address{"file_name"};

        CHECK(address.to_string() == "shm://file_name");
    }

    SECTION("compare with other addresses") {
        const PosixSharedMemoryAddress address1{"file_name1"};
        const PosixSharedMemoryAddress address2{
            "file_name1"};  // same as address1
        const PosixSharedMemoryAddress address3{"file_name3"};

        CHECK(address1 == address2);
        CHECK_FALSE(address1 == address3);
        CHECK_FALSE(address1 != address2);
        CHECK(address1 != address3);
    }

    SECTION("format using fmt library") {
        const PosixSharedMemoryAddress address{"file_name"};

        CHECK(fmt::format("{}", address) == "shm://file_name");
    }

    SECTION("format using ostream") {
        const PosixSharedMemoryAddress address{"file_name"};

        std::ostringstream stream;
        stream << address;
        CHECK(stream.str() == "shm://file_name");
    }
}

#endif
