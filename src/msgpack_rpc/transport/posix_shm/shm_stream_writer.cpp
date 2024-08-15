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
 * \brief Implementation of ShmStreamWriter class.
 */
#include "msgpack_rpc/transport/posix_shm/shm_stream_writer.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <boost/atomic/ipc_atomic.hpp>
#include <boost/memory_order.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/messages/buffer_view.h"

namespace msgpack_rpc::transport::posix_shm {

ShmStreamWriter::ShmStreamWriter(AtomicIndex* atomic_next_written_index,
    AtomicIndex* atomic_next_read_index, messages::BufferView buffer)
    : atomic_next_written_index_(atomic_next_written_index),
      atomic_next_read_index_(atomic_next_read_index),
      buffer_(buffer.data()),
      buffer_size_(static_cast<Index>(buffer.size())),
      next_written_index_(atomic_next_written_index->load()) {
    if (buffer.size() > MAX_BUFFER_SIZE) {
        throw MsgpackRPCException(
            StatusCode::INVALID_ARGUMENT, "Too large buffer on shared memory.");
    }
}

std::size_t ShmStreamWriter::write_some(
    const char* data, std::size_t data_size) noexcept {
    const Index next_read_index =
        atomic_next_read_index_->load(boost::memory_order_acquire);
    std::size_t written_bytes = 0U;
    while (written_bytes < data_size) {
        const Index writable_bytes =
            calc_continuously_writable_size(next_read_index);
        if (writable_bytes == 0U) {
            break;
        }
        const std::size_t remaining_data_bytes = data_size - written_bytes;
        const Index write_size = static_cast<Index>(
            std::min<std::size_t>(writable_bytes, remaining_data_bytes));
        std::memcpy(
            buffer_ + next_written_index_, data + written_bytes, write_size);
        next_written_index_ += write_size;
        next_written_index_ %= buffer_size_;
        written_bytes += write_size;
    }
    atomic_next_written_index_->store(
        next_written_index_, boost::memory_order_release);
    return written_bytes;
}

ShmStreamWriter::Index ShmStreamWriter::calc_continuously_writable_size(
    Index next_read_index) const noexcept {
    if (next_written_index_ < next_read_index) {
        return next_read_index - next_written_index_ - 1U;
    }
    if (next_read_index == 0U) {
        return buffer_size_ - next_written_index_ - 1U;
    }
    return buffer_size_ - next_written_index_;
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
