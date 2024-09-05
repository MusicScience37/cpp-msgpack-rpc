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

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <algorithm>
#include <cstddef>
#include <cstring>

#include <boost/memory_order.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/messages/buffer_view.h"

namespace msgpack_rpc::transport::posix_shm {

ShmStreamReader::ShmStreamReader(AtomicIndex* atomic_next_written_index,
    AtomicIndex* atomic_next_read_index, messages::BufferView buffer)
    : atomic_next_written_index_(atomic_next_written_index),
      atomic_next_read_index_(atomic_next_read_index),
      buffer_(buffer.data()),
      buffer_size_(static_cast<Index>(buffer.size())),
      next_read_index_(
          atomic_next_read_index_->load(boost::memory_order_acquire)) {
    if (buffer.size() > MAX_BUFFER_SIZE) {
        throw MsgpackRPCException(
            StatusCode::INVALID_ARGUMENT, "Too large buffer on shared memory.");
    }
}

std::size_t ShmStreamReader::read_some(
    char* data, std::size_t data_size) noexcept {
    const Index next_written_index =
        atomic_next_written_index_->load(boost::memory_order_acquire);
    std::size_t total_read_size = 0U;
    while (total_read_size < data_size) {
        const Index readable_size =
            calc_continuously_readable_size(next_written_index);
        if (readable_size == 0U) {
            break;
        }
        const std::size_t remaining_data_bytes = data_size - total_read_size;
        const Index read_size = static_cast<Index>(
            std::min<std::size_t>(readable_size, remaining_data_bytes));
        std::memcpy(
            data + total_read_size, buffer_ + next_read_index_, read_size);
        next_read_index_ += read_size;
        next_read_index_ %= buffer_size_;
        total_read_size += read_size;
    }
    atomic_next_read_index_->store(
        next_read_index_, boost::memory_order_release);
    return total_read_size;
}

ShmStreamReader::Index ShmStreamReader::calc_continuously_readable_size(
    Index next_written_index) const noexcept {
    if (next_read_index_ <= next_written_index) {
        return next_written_index - next_read_index_;
    }
    return buffer_size_ - next_read_index_;
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
