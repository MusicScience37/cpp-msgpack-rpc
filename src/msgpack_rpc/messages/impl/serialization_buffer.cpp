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
 * \brief Implementation of SerializationBuffer class.
 */
#include "msgpack_rpc/messages/impl/serialization_buffer.h"

#include <cstddef>
#include <cstring>

#include "msgpack_rpc/messages/impl/sharable_binary_header.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::messages::impl {

SerializationBuffer::SerializationBuffer()
    : buffer_(allocate_shared_binary()) {}

SerializationBuffer::~SerializationBuffer() noexcept {
    deallocate_sharable_binary(buffer_);
}

void SerializationBuffer::write(const char* data, std::size_t size) {
    if (buffer_->binary_size + size > buffer_->binary_capacity) {
        buffer_ = expand_sharable_binary(buffer_, buffer_->binary_size + size);
    }
    std::memcpy(binary_buffer_of(buffer_) + buffer_->binary_size, data, size);
    buffer_->binary_size += size;
}

SerializedMessage SerializationBuffer::release() noexcept {
    enable_reference_count_of_sharable_binary(buffer_);
    SerializedMessage message{buffer_};
    buffer_ = nullptr;
    return message;
}

}  // namespace msgpack_rpc::messages::impl
