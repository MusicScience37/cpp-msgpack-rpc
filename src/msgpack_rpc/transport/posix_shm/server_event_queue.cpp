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
 * \brief Implementation of ServerEventQueue class.
 */
#include "msgpack_rpc/transport/posix_shm/server_event_queue.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>
#include <mutex>
#include <optional>

#include <boost/atomic/ipc_atomic.hpp>
#include <boost/memory_order.hpp>

#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

namespace msgpack_rpc::transport::posix_shm {

ServerEventQueue::ServerEventQueue(PosixShmMutexView writer_mutex,
    AtomicIndex* atomic_next_written_index, AtomicIndex* atomic_next_read_index,
    ServerEvent* buffer, std::size_t buffer_size)
    : writer_mutex_(writer_mutex),
      atomic_next_written_index_(atomic_next_written_index),
      atomic_next_read_index_(atomic_next_read_index),
      buffer_(buffer),
      buffer_size_(buffer_size) {}

bool ServerEventQueue::push(const ServerEvent& event) noexcept {
    std::unique_lock<PosixShmMutexView> lock(writer_mutex_);

    const Index next_written_index =
        atomic_next_written_index_->load(boost::memory_order_acquire);
    const Index next_read_index =
        atomic_next_read_index_->load(boost::memory_order_acquire);
    const Index incremented_next_written_index =
        (next_written_index + 1) % buffer_size_;
    if (incremented_next_written_index == next_read_index) {
        return false;
    }

    buffer_[next_written_index] = event;

    atomic_next_written_index_->store(
        incremented_next_written_index, boost::memory_order_release);

    return true;
}

std::optional<ServerEvent> ServerEventQueue::pop() noexcept {
    const Index next_read_index =
        atomic_next_read_index_->load(boost::memory_order_acquire);
    const Index next_written_index =
        atomic_next_written_index_->load(boost::memory_order_acquire);
    if (next_read_index == next_written_index) {
        return std::nullopt;
    }

    const ServerEvent event = buffer_[next_read_index];

    atomic_next_read_index_->store(
        (next_read_index + 1) % buffer_size_, boost::memory_order_release);

    return event;
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
