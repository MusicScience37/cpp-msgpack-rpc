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
 * \brief Test of ServerMemoryAddressCalculator class.
 */
#include "msgpack_rpc/transport/posix_shm/server_memory_address_calculator.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <chrono>
#include <cstddef>
#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/transport/posix_shm/server_event_queue.h"
#include "msgpack_rpc/transport/posix_shm/server_state.h"

TEST_CASE("msgpack_rpc::transport::posix_shm::ServerMemoryAddressCalculator") {
    using msgpack_rpc::transport::posix_shm::ServerEvent;
    using msgpack_rpc::transport::posix_shm::ServerEventQueue;
    using msgpack_rpc::transport::posix_shm::ServerEventType;
    using msgpack_rpc::transport::posix_shm::ServerMemoryAddressCalculator;
    using msgpack_rpc::transport::posix_shm::ServerMemoryParameters;
    using msgpack_rpc::transport::posix_shm::ServerState;

    SECTION("calculate parameters") {
        constexpr std::size_t event_queue_buffer_size = 3;

        const ServerMemoryParameters parameters =
            ServerMemoryAddressCalculator::calculate_parameters(
                event_queue_buffer_size);

        CHECK(parameters.server_state_address == 64U);
        CHECK(parameters.event_queue_address == 128U);
        CHECK(parameters.event_queue_buffer_size == event_queue_buffer_size);
        CHECK(parameters.total_memory_size == 248U);

        std::vector<char> memory(parameters.total_memory_size);
        std::memcpy(memory.data(), &parameters, sizeof(parameters));
        ServerMemoryAddressCalculator calculator{memory.data()};

        SECTION("get parameters") {
            const auto* result = calculator.parameters();

            CHECK(result->server_state_address == 64U);
            CHECK(result->event_queue_address == 128U);
            CHECK(result->event_queue_buffer_size == event_queue_buffer_size);
            CHECK(result->total_memory_size == 248U);
        }

        SECTION("get the state of the server") {
            const auto* result = calculator.server_state();
            CHECK(result->load() == static_cast<ServerState>(0));
        }

        SECTION("create event queue") {
            ServerEventQueue queue = calculator.server_event_queue();

            queue.initialize();
            REQUIRE(queue.push(
                ServerEvent{1, ServerEventType::CLIENT_STATE_CHANGED},
                std::chrono::seconds(1)));
            const auto popped_event = queue.pop(std::chrono::seconds(1));
            REQUIRE(popped_event.has_value());
            CHECK(popped_event->client_id == 1);
            CHECK(popped_event->type == ServerEventType::CLIENT_STATE_CHANGED);
        }
    }
}

#endif
