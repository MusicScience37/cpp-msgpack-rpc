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
 * \brief Test of ClientMemoryAddressCalculator class.
 */
#include "msgpack_rpc/transport/posix_shm/client_memory_address_calculator.h"

#include <cstring>
#include <string_view>

#include "msgpack_rpc/config.h"
#include "msgpack_rpc/transport/posix_shm/clint_state.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::transport::posix_shm::ClientMemoryAddressCalculator") {
    using msgpack_rpc::transport::posix_shm::ClientMemoryAddressCalculator;
    using msgpack_rpc::transport::posix_shm::ClientMemoryParameters;
    using msgpack_rpc::transport::posix_shm::ClientState;

    SECTION("calculate parameters") {
        constexpr std::size_t stream_buffer_size = 15;

        const ClientMemoryParameters parameters =
            ClientMemoryAddressCalculator::calculate_parameters(
                stream_buffer_size);

        CHECK(parameters.changes_count_address == 64U);
        CHECK(parameters.client_state_address == 128U);
        CHECK(parameters.client_to_server_stream_address == 192U);
        CHECK(parameters.server_to_client_stream_address == 256U);
        CHECK(parameters.stream_buffer_size == stream_buffer_size);
        CHECK(parameters.total_memory_size == 279U);

        std::vector<char> memory(parameters.total_memory_size);
        std::memcpy(memory.data(), &parameters, sizeof(parameters));
        ClientMemoryAddressCalculator calculator{memory.data()};

        SECTION("get parameters") {
            const auto* result = calculator.parameters();

            CHECK(result->changes_count_address == 64U);
            CHECK(result->client_state_address == 128U);
            CHECK(result->client_to_server_stream_address == 192U);
            CHECK(result->server_to_client_stream_address == 256U);
            CHECK(result->stream_buffer_size == stream_buffer_size);
            CHECK(result->total_memory_size == 279U);
        }

        SECTION("get count of changes") {
            auto* result = calculator.changes_count();

            CHECK(result->load() == 0U);
            result->store(123U);  // NOLINT(*-magic-numbers)
            CHECK(result->load() == 123U);
        }

        SECTION("get the state of the client") {
            auto* result = calculator.client_state();

            CHECK(result->load() == static_cast<ClientState>(0));
            result->store(ClientState::CONNECTED);
            CHECK(result->load() == ClientState::CONNECTED);
        }

        SECTION("use stream from client to server") {
            auto writer = calculator.client_to_server_stream_writer();
            auto reader = calculator.client_to_server_stream_reader();

            const std::string written_data = "abc";
            REQUIRE(writer.write_some(written_data.data(),
                        written_data.size()) == written_data.size());

            std::vector<char> read_data(written_data.size());
            REQUIRE(reader.read_some(read_data.data(), read_data.size()) ==
                read_data.size());
            CHECK(std::string_view{read_data.data(), read_data.size()} ==
                written_data);
        }

        SECTION("use stream from server to client") {
            auto writer = calculator.server_to_client_stream_writer();
            auto reader = calculator.server_to_client_stream_reader();

            const std::string written_data = "abc";
            REQUIRE(writer.write_some(written_data.data(),
                        written_data.size()) == written_data.size());

            std::vector<char> read_data(written_data.size());
            REQUIRE(reader.read_some(read_data.data(), read_data.size()) ==
                read_data.size());
            CHECK(std::string_view{read_data.data(), read_data.size()} ==
                written_data);
        }
    }
}

#endif
