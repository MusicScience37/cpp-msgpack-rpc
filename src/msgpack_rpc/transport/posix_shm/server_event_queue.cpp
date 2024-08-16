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

#include <chrono>
#include <cstddef>
#include <mutex>
#include <optional>

#include <boost/atomic/ipc_atomic.hpp>
#include <boost/memory_order.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

namespace msgpack_rpc::transport::posix_shm {

ServerEventQueue::ServerEventQueue(PosixShmMutexView mutex,
    PosixShmConditionVariableView condition_variable, Index* next_written_index,
    Index* next_read_index, ServerEvent* buffer, std::size_t buffer_size)
    : mutex_(mutex),
      condition_variable_(condition_variable),
      next_written_index_(next_written_index),
      next_read_index_(next_read_index),
      buffer_(buffer),
      buffer_size_(static_cast<Index>(buffer_size)) {
    if (buffer_size > MAX_BUFFER_SIZE) {
        throw MsgpackRPCException(
            StatusCode::INVALID_ARGUMENT, "Too large size of the buffer.");
    }
}

void ServerEventQueue::initialize() {
    mutex_.initialize();
    condition_variable_.initialize();
    *next_written_index_ = 0;
    *next_read_index_ = 0;
}

bool ServerEventQueue::push(
    const ServerEvent& event, std::chrono::nanoseconds timeout) noexcept {
    std::unique_lock<PosixShmMutexView> lock(mutex_);

    if (!condition_variable_.wait_for(lock, timeout, [this] {
            return ((*next_written_index_ + 1) % buffer_size_) !=
                *next_read_index_;
        })) {
        return false;
    }

    buffer_[*next_written_index_] = event;

    *next_written_index_ = (*next_written_index_ + 1) % buffer_size_;

    return true;
}

std::optional<ServerEvent> ServerEventQueue::pop(
    std::chrono::nanoseconds timeout) noexcept {
    std::unique_lock<PosixShmMutexView> lock(mutex_);

    if (!condition_variable_.wait_for(lock, timeout,
            [this] { return *next_read_index_ != *next_written_index_; })) {
        return std::nullopt;
    }

    const ServerEvent event = buffer_[*next_read_index_];

    *next_read_index_ = (*next_read_index_ + 1) % buffer_size_;

    return event;
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
