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
 * \brief Implementation of MessageParserConfig class.
 */
#include "msgpack_rpc/config/message_parser_config.h"

#include <string_view>

#include <msgpack.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::config {

MessageParserConfig::MessageParserConfig()
    : read_buffer_size_(
          static_cast<std::size_t>(MSGPACK_UNPACKER_RESERVE_SIZE)) {}

MessageParserConfig& MessageParserConfig::read_buffer_size(std::size_t value) {
    if (value <= 0U) {
        throw MsgpackRPCException(
            StatusCode::INVALID_ARGUMENT, "Buffer size must be at least one.");
    }
    read_buffer_size_ = value;
    return *this;
}

std::size_t MessageParserConfig::read_buffer_size() const noexcept {
    return read_buffer_size_;
}

}  // namespace msgpack_rpc::config
