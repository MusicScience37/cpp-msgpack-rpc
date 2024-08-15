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
 * \brief Test of ShmStreamWriter class.
 */
#include "msgpack_rpc/transport/posix_shm/shm_stream_writer.h"

#include <string_view>

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <vector>

#include <boost/atomic/ipc_atomic.hpp>
#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/messages/buffer_view.h"

TEST_CASE("msgpack_rpc::transport::posix_shm::ShmStreamWriter") {
    using msgpack_rpc::messages::BufferView;
    using msgpack_rpc::transport::posix_shm::ShmStreamWriter;

    ShmStreamWriter::AtomicIndex atomic_next_written_index{0};
    ShmStreamWriter::AtomicIndex atomic_next_read_index{0};
    constexpr std::size_t buffer_size = 7;
    std::vector<char> buffer(buffer_size);

    SECTION("for an empty buffer") {
        atomic_next_written_index.store(0U);
        atomic_next_read_index.store(0U);
        ShmStreamWriter writer{&atomic_next_written_index,
            &atomic_next_read_index, BufferView{buffer.data(), buffer.size()}};

        SECTION("write data leaving some room for other data") {
            constexpr auto data = std::string_view("abcde");

            CHECK(writer.write_some(data.data(), data.size()) == data.size());

            CHECK(atomic_next_written_index.load() == 5U);
            CHECK(atomic_next_read_index.load() == 0U);
            CHECK(std::string_view(buffer.data(), 5U) == data);
        }

        SECTION("write data leaving no room for other data") {
            constexpr auto data = std::string_view("abcdef");

            CHECK(writer.write_some(data.data(), data.size()) == data.size());

            CHECK(atomic_next_written_index.load() == 6U);
            CHECK(atomic_next_read_index.load() == 0U);
            CHECK(std::string_view(buffer.data(), 6U) == data);
        }

        SECTION("write data with insufficient room in buffer") {
            constexpr auto data = std::string_view("abcdefg");

            CHECK(writer.write_some(data.data(), data.size()) == 6U);

            CHECK(atomic_next_written_index.load() == 6U);
            CHECK(atomic_next_read_index.load() == 0U);
            CHECK(std::string_view(buffer.data(), 6U) == "abcdef");
        }
    }

    SECTION(
        "for an already used buffer with atomic_next_written_index < "
        "atomic_next_read_index") {
        atomic_next_written_index.store(1U);
        atomic_next_read_index.store(5U);  // NOLINT(*-magic-numbers)
        ShmStreamWriter writer{&atomic_next_written_index,
            &atomic_next_read_index, BufferView{buffer.data(), buffer.size()}};

        SECTION("write data leaving some room for other data") {
            constexpr auto data = std::string_view("ab");

            CHECK(writer.write_some(data.data(), data.size()) == data.size());

            CHECK(atomic_next_written_index.load() == 3U);
            CHECK(atomic_next_read_index.load() == 5U);
            CHECK(std::string_view(buffer.data() + 1, 2U) == data);
        }

        SECTION("write data leaving no room for other data") {
            constexpr auto data = std::string_view("abc");

            CHECK(writer.write_some(data.data(), data.size()) == data.size());

            CHECK(atomic_next_written_index.load() == 4U);
            CHECK(atomic_next_read_index.load() == 5U);
            CHECK(std::string_view(buffer.data() + 1, 3U) == data);
        }

        SECTION("write data with insufficient room in buffer") {
            constexpr auto data = std::string_view("abcd");

            CHECK(writer.write_some(data.data(), data.size()) == 3U);

            CHECK(atomic_next_written_index.load() == 4U);
            CHECK(atomic_next_read_index.load() == 5U);
            CHECK(std::string_view(buffer.data() + 1, 3U) == "abc");
        }
    }

    SECTION(
        "for an already used buffer with atomic_next_written_index > "
        "atomic_next_read_index") {
        atomic_next_written_index.store(5U);  // NOLINT(*-magic-numbers)
        atomic_next_read_index.store(4U);
        ShmStreamWriter writer{&atomic_next_written_index,
            &atomic_next_read_index, BufferView{buffer.data(), buffer.size()}};

        SECTION("write data leaving some room for other data") {
            constexpr auto data = std::string_view("abcd");

            CHECK(writer.write_some(data.data(), data.size()) == data.size());

            CHECK(atomic_next_written_index.load() == 2U);
            CHECK(atomic_next_read_index.load() == 4U);
            CHECK(std::string_view(buffer.data() + 5, 2U) == "ab");
            CHECK(std::string_view(buffer.data(), 2U) == "cd");
        }

        SECTION("write data leaving no room for other data") {
            constexpr auto data = std::string_view("abcde");

            CHECK(writer.write_some(data.data(), data.size()) == data.size());

            CHECK(atomic_next_written_index.load() == 3U);
            CHECK(atomic_next_read_index.load() == 4U);
            CHECK(std::string_view(buffer.data() + 5, 2U) == "ab");
            CHECK(std::string_view(buffer.data(), 3U) == "cde");
        }

        SECTION("write data with insufficient room in buffer") {
            constexpr auto data = std::string_view("abcdef");

            CHECK(writer.write_some(data.data(), data.size()) == 5U);

            CHECK(atomic_next_written_index.load() == 3U);
            CHECK(atomic_next_read_index.load() == 4U);
            CHECK(std::string_view(buffer.data() + 5, 2U) == "ab");
            CHECK(std::string_view(buffer.data(), 3U) == "cde");
        }
    }
}

#endif
