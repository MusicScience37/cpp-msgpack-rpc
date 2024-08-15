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
 * \brief Implementation of ShmStreamReader class.
 */
#include "msgpack_rpc/transport/posix_shm/shm_stream_reader.h"

#include <string_view>

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <string>
#include <vector>

#include <boost/atomic/ipc_atomic.hpp>
#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/messages/buffer_view.h"

TEST_CASE("msgpack_rpc::transport::posix_shm::ShmStreamReader") {
    using msgpack_rpc::messages::BufferView;
    using msgpack_rpc::transport::posix_shm::ShmStreamReader;

    ShmStreamReader::AtomicIndex atomic_next_written_index{0};
    ShmStreamReader::AtomicIndex atomic_next_read_index{0};
    std::string buffer = "abcdefg";

    std::vector<char> data(100U, 'z');  // NOLINT(*-magic-numbers)

    SECTION("for an empty buffer") {
        atomic_next_written_index.store(3U);
        atomic_next_read_index.store(3U);
        ShmStreamReader reader{&atomic_next_written_index,
            &atomic_next_read_index, BufferView{buffer.data(), buffer.size()}};

        SECTION("try to read data") {
            std::vector<char> data(1U);

            CHECK(reader.read_some(data.data(), data.size()) == 0U);
            CHECK(atomic_next_written_index.load() == 3U);
            CHECK(atomic_next_read_index.load() == 3U);
        }
    }

    SECTION(
        "for a buffer with atomic_next_read_index < "
        "atomic_next_written_index") {
        atomic_next_written_index.store(5U);  // NOLINT(*-magic-numbers)
        atomic_next_read_index.store(2U);
        ShmStreamReader reader{&atomic_next_written_index,
            &atomic_next_read_index, BufferView{buffer.data(), buffer.size()}};

        SECTION("read a part of data") {
            CHECK(reader.read_some(data.data(), 2U) == 2U);
            CHECK(atomic_next_written_index.load() == 5U);
            CHECK(atomic_next_read_index.load() == 4U);
            CHECK(std::string_view(data.data(), 2U) == "cd");
        }

        SECTION("read all data") {
            CHECK(reader.read_some(data.data(), 3U) == 3U);
            CHECK(atomic_next_written_index.load() == 5U);
            CHECK(atomic_next_read_index.load() == 5U);
            CHECK(std::string_view(data.data(), 3U) == "cde");
        }

        SECTION("try to read too large data") {
            CHECK(reader.read_some(data.data(), 4U) == 3U);
            CHECK(atomic_next_written_index.load() == 5U);
            CHECK(atomic_next_read_index.load() == 5U);
            // cspell: ignore cdez
            CHECK(std::string_view(data.data(), 4U) == "cdez");
        }
    }

    SECTION(
        "for a buffer with atomic_next_read_index > "
        "atomic_next_written_index") {
        atomic_next_written_index.store(1U);
        atomic_next_read_index.store(5U);  // NOLINT(*-magic-numbers)
        ShmStreamReader reader{&atomic_next_written_index,
            &atomic_next_read_index, BufferView{buffer.data(), buffer.size()}};

        SECTION("read a part of data") {
            CHECK(reader.read_some(data.data(), 2U) == 2U);
            CHECK(atomic_next_written_index.load() == 1U);
            CHECK(atomic_next_read_index.load() == 0U);
            CHECK(std::string_view(data.data(), 2U) == "fg");
        }

        SECTION("read all data") {
            CHECK(reader.read_some(data.data(), 3U) == 3U);
            CHECK(atomic_next_written_index.load() == 1U);
            CHECK(atomic_next_read_index.load() == 1U);
            CHECK(std::string_view(data.data(), 3U) == "fga");
        }

        SECTION("try to read too large data") {
            CHECK(reader.read_some(data.data(), 4U) == 3U);
            CHECK(atomic_next_written_index.load() == 1U);
            CHECK(atomic_next_read_index.load() == 1U);
            // cspell: ignore fgaz
            CHECK(std::string_view(data.data(), 4U) == "fgaz");
        }
    }
}

#endif
