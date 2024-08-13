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
 * \brief Implementation of PosixSharedMemory class.
 */
#include "msgpack_rpc/transport/posix_shm/posix_shared_memory.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstring>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::transport::posix_shm::PosixSharedMemory") {
    using msgpack_rpc::transport::posix_shm::PosixSharedMemory;

    SECTION("share data") {
        constexpr std::string_view file_name =
            "msgpack_rpc_test_units_posix_shared_memory";
        constexpr std::string_view data = "abcdefghijklmnopqrstuvwxyz";

        {
            INFO("Open and write data.");
            PosixSharedMemory shared_memory{
                file_name, data.size(), PosixSharedMemory::INITIALIZE};
            REQUIRE(shared_memory.size() == data.size());
            std::memcpy(shared_memory.get(), data.data(), data.size());
        }

        {
            INFO("Open and read data.");
            PosixSharedMemory shared_memory{
                file_name, PosixSharedMemory::OPEN_EXISTING};
            REQUIRE(shared_memory.size() == data.size());
            CHECK(
                std::string_view(static_cast<const char*>(shared_memory.get()),
                    shared_memory.size()) == data);
        }

        CHECK(PosixSharedMemory::remove(file_name));
    }
}

#endif
