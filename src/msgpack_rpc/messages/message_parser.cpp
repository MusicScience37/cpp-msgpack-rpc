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
 * \brief Implementation of MessageParser class.
 */
#include "msgpack_rpc/messages/message_parser.h"

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/messages/impl/parse_message_from_object.h"
#include "msgpack_rpc/messages/message_type.h"
#include "msgpack_rpc/messages/parsed_message.h"

namespace msgpack_rpc::messages {

MessageParser::MessageParser(const config::MessageParserConfig& /*config*/) {}

MessageParser::~MessageParser() = default;

BufferView MessageParser::prepare_buffer() {
    parser_.reserve_buffer(read_buffer_size_);
    return BufferView(parser_.buffer(), read_buffer_size_);
}

void MessageParser::consumed(std::size_t num_bytes) {
    parser_.buffer_consumed(num_bytes);
}

std::optional<ParsedMessage> MessageParser::try_parse() {
    msgpack::object_handle object;
    try {
        if (!parser_.next(object)) {
            return std::nullopt;
        }
    } catch (const msgpack::unpack_error&) {
        throw MsgpackRPCException(
            StatusCode::INVALID_MESSAGE, "Failed to parse a message.");
    }

    return impl::parse_message_from_object(std::move(object));
}

}  // namespace msgpack_rpc::messages
