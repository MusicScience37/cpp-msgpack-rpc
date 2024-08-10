/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of SerializedMessage class.
 */
#include "msgpack_rpc/messages/serialized_message.h"

#include <cstddef>
#include <cstring>
#include <utility>

#include "msgpack_rpc/messages/impl/sharable_binary_header.h"

namespace msgpack_rpc::messages {

SerializedMessage::SerializedMessage(const char* data, std::size_t size)
    : buffer_(impl::allocate_sharable_binary(size)) {
    // Following operations won't throw exceptions.
    impl::enable_reference_count_of_sharable_binary(buffer_);
    std::memcpy(impl::binary_buffer_of(buffer_), data, size);
}

SerializedMessage::SerializedMessage(impl::SharableBinaryHeader* buffer)
    : buffer_(buffer) {}

SerializedMessage::SerializedMessage(const SerializedMessage& other) noexcept
    : buffer_(other.buffer_) {
    impl::add_reference_count_of_sharable_binary(buffer_);
}

SerializedMessage::SerializedMessage(SerializedMessage&& other) noexcept
    : buffer_(std::exchange(other.buffer_, nullptr)) {}

SerializedMessage& SerializedMessage::operator=(
    const SerializedMessage& other) noexcept {
    if (this == &other) {
        return *this;
    }
    if (buffer_ != nullptr) {
        impl::decrease_reference_count_of_sharable_binary(buffer_);
    }
    buffer_ = other.buffer_;
    impl::add_reference_count_of_sharable_binary(buffer_);
    return *this;
}

SerializedMessage& SerializedMessage::operator=(
    SerializedMessage&& other) noexcept {
    std::swap(buffer_, other.buffer_);
    return *this;
}

SerializedMessage::~SerializedMessage() noexcept {
    if (buffer_ != nullptr) {
        impl::decrease_reference_count_of_sharable_binary(buffer_);
    }
}

const char* SerializedMessage::data() const noexcept {
    return impl::binary_buffer_of(buffer_);
}

std::size_t SerializedMessage::size() const noexcept {
    return buffer_->binary_size;
}

}  // namespace msgpack_rpc::messages
