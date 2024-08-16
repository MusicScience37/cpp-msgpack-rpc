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

#include <vector>

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

TEST_CASE("msgpack_rpc::transport::posix_shm::ServerEventQueue") {
    using msgpack_rpc::transport::posix_shm::PosixShmMutexView;
    using msgpack_rpc::transport::posix_shm::ServerEvent;
    using msgpack_rpc::transport::posix_shm::ServerEventQueue;
    using msgpack_rpc::transport::posix_shm::ServerEventType;

    PosixShmMutexView::ActualMutex actual_writer_mutex;
    PosixShmMutexView writer_mutex(&actual_writer_mutex);

    ServerEventQueue::AtomicIndex atomic_next_written_index;
    ServerEventQueue::AtomicIndex atomic_next_read_index;
    std::vector<ServerEvent> buffer(4);  // NOLINT(*-magic-numbers)

    ServerEventQueue queue(writer_mutex, &atomic_next_written_index,
        &atomic_next_read_index, buffer.data(), buffer.size());
    queue.initialize();

    SECTION("push and pop an event") {
        const ServerEvent event{1, ServerEventType::CLIENT_CREATED};

        REQUIRE(queue.push(event));

        const auto popped_event = queue.pop();

        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == event.client_id);
        CHECK(popped_event.value().type == event.type);
    }

    SECTION("push too many events") {
        REQUIRE(queue.push(ServerEvent{1, ServerEventType::CLIENT_CREATED}));
        REQUIRE(
            queue.push(ServerEvent{2, ServerEventType::CLIENT_STATE_CHANGED}));
        REQUIRE(queue.push(ServerEvent{3, ServerEventType::CLIENT_DESTROYED}));
        REQUIRE_FALSE(
            queue.push(ServerEvent{4, ServerEventType::CLIENT_STATE_CHANGED}));

        auto popped_event = queue.pop();
        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == 1);
        CHECK(popped_event.value().type == ServerEventType::CLIENT_CREATED);

        popped_event = queue.pop();
        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == 2);
        CHECK(
            popped_event.value().type == ServerEventType::CLIENT_STATE_CHANGED);

        popped_event = queue.pop();
        REQUIRE(popped_event.has_value());
        CHECK(popped_event.value().client_id == 3);
        CHECK(popped_event.value().type == ServerEventType::CLIENT_DESTROYED);
    }
}

#endif
