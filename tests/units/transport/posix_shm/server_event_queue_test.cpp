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
 * \brief Test of ServerEventQueue class.
 */
#include "msgpack_rpc/transport/posix_shm/server_event_queue.h"

#include <chrono>

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

TEST_CASE("msgpack_rpc::transport::posix_shm::ServerEventQueue") {
    using msgpack_rpc::transport::posix_shm::PosixShmConditionVariableView;
    using msgpack_rpc::transport::posix_shm::PosixShmMutexView;
    using msgpack_rpc::transport::posix_shm::ServerEvent;
    using msgpack_rpc::transport::posix_shm::ServerEventQueue;
    using msgpack_rpc::transport::posix_shm::ServerEventType;

    PosixShmMutexView::ActualMutex actual_mutex;
    PosixShmMutexView mutex(&actual_mutex);

    ServerEventQueue::Index next_written_index = 0;
    ServerEventQueue::Index next_read_index = 0;

    PosixShmConditionVariableView::ActualConditionVariable
        actual_condition_variable;
    PosixShmConditionVariableView condition_variable(
        &actual_condition_variable);

    std::vector<ServerEvent> buffer(4);  // NOLINT(*-magic-numbers)

    ServerEventQueue queue(mutex, condition_variable, &next_written_index,
        &next_read_index, buffer.data(), buffer.size());
    queue.initialize();

    constexpr std::chrono::seconds long_timeout{1};
    constexpr std::chrono::milliseconds short_timeout{10};

    SECTION("push and pop an event") {
        const ServerEvent event{1, ServerEventType::CLIENT_CREATED};

        REQUIRE(queue.push(event, long_timeout));

        const auto popped_event = queue.pop(long_timeout);

        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == event.client_id);
        CHECK(popped_event.value().type == event.type);
    }

    SECTION("push too many events") {
        REQUIRE(queue.push(
            ServerEvent{1, ServerEventType::CLIENT_CREATED}, long_timeout));
        REQUIRE(
            queue.push(ServerEvent{2, ServerEventType::CLIENT_STATE_CHANGED},
                long_timeout));
        REQUIRE(queue.push(
            ServerEvent{3, ServerEventType::CLIENT_DESTROYED}, long_timeout));
        REQUIRE_FALSE(
            queue.push(ServerEvent{4, ServerEventType::CLIENT_STATE_CHANGED},
                short_timeout));

        auto popped_event = queue.pop(long_timeout);
        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == 1);
        CHECK(popped_event.value().type == ServerEventType::CLIENT_CREATED);

        popped_event = queue.pop(long_timeout);
        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == 2);
        CHECK(
            popped_event.value().type == ServerEventType::CLIENT_STATE_CHANGED);

        popped_event = queue.pop(long_timeout);
        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == 3);
        CHECK(popped_event.value().type == ServerEventType::CLIENT_DESTROYED);
    }

    SECTION("pop without events") {
        REQUIRE_FALSE(queue.pop(short_timeout).has_value());
    }
}

#endif
