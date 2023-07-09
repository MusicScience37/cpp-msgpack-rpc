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
 * \brief Definition of MessageType enumeration.
 */
#pragma once

#include <string_view>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Enumeration of message types.
 *
 * \note Values are same as values in MessagePack-RPC specification.
 */
enum class MessageType {
    //! Request.
    REQUEST = 0,

    //! Response.
    RESPONSE = 1,

    //! Notification.
    NOTIFICATION = 2
};

/*!
 * \brief Format a message type.
 *
 * \param[in] type Message type.
 * \return Formatted string.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::string_view format_message_type(
    MessageType type) noexcept;

}  // namespace msgpack_rpc::messages
