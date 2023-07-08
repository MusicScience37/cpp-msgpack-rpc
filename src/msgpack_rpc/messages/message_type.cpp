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
 * \brief Implementation of functions for message types.
 */
#include "msgpack_rpc/messages/message_type.h"

namespace msgpack_rpc::messages {

std::string_view format_message_type(MessageType type) noexcept {
    switch (type) {
    case MessageType::REQUEST:
        return "REQUEST";
    case MessageType::RESPONSE:
        return "RESPONSE";
    case MessageType::NOTIFICATION:
        return "NOTIFICATION";
    }
    return "INVALID_MESSAGE_TYPE";
}

}  // namespace msgpack_rpc::messages
