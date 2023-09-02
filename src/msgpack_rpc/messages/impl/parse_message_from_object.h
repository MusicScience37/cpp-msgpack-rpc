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
 * \brief Definition of parse_message_from_object function.
 */
#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string_view>
#include <utility>

#include <msgpack.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_type.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_parameters.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/parsed_response.h"

namespace msgpack_rpc::messages::impl {

/*!
 * \brief Parse a message type from an object in msgpack library.
 *
 * \param[in] object Object in msgpack library.
 * \return Message type.
 */
[[nodiscard]] inline MessageType parse_message_type_from_object(
    const msgpack::object& object) {
    try {
        const auto val = object.as<std::uint64_t>();
        switch (val) {
        case 0U:
            return MessageType::REQUEST;
        case 1U:
            return MessageType::RESPONSE;
        case 2U:
            return MessageType::NOTIFICATION;
        default:
            throw MsgpackRPCException(StatusCode::INVALID_MESSAGE,
                "Invalid message type in a message.");
        }
    } catch (const msgpack::type_error&) {
        throw MsgpackRPCException(
            StatusCode::INVALID_MESSAGE, "Invalid message type in a message.");
    }
}

/*!
 * \brief Parse a message ID  from an object in msgpack library.
 *
 * \param[in] object Object in msgpack library.
 * \return Message ID.
 */
[[nodiscard]] inline MessageID parse_message_id_from_object(
    const msgpack::object& object) {
    try {
        return object.as<MessageID>();
    } catch (const msgpack::type_error&) {
        throw MsgpackRPCException(
            StatusCode::INVALID_MESSAGE, "Invalid message ID in a message.");
    }
}

/*!
 * \brief Parse a method name from an object in msgpack library.
 *
 * \param[in] object Object in msgpack library.
 * \return Method name.
 */
[[nodiscard]] inline MethodNameView parse_method_name_from_object(
    const msgpack::object& object) {
    try {
        return MethodNameView(object.as<std::string_view>());
    } catch (const msgpack::type_error&) {
        throw MsgpackRPCException(
            StatusCode::INVALID_MESSAGE, "Invalid method name in a message.");
    }
}

/*!
 * \brief Parse a request from an object in msgpack library.
 *
 * \param[in] object Object in msgpack library.
 * \return Request.
 */
[[nodiscard]] inline ParsedRequest parse_request_from_object(
    msgpack::object_handle object) {
    constexpr std::uint32_t num_elements_in_root_array = 4;
    if (object->via.array.size != num_elements_in_root_array) {
        throw MsgpackRPCException(StatusCode::INVALID_MESSAGE,
            "Invalid size of the array of a message.");
    }

    const auto message_id =
        parse_message_id_from_object(object->via.array.ptr[1]);
    auto method_name = parse_method_name_from_object(object->via.array.ptr[2]);
    auto parameters =
        ParsedParameters(object->via.array.ptr[3], std::move(object.zone()));
    return ParsedRequest(message_id, method_name, std::move(parameters));
}

/*!
 * \brief Parse a response from an object in msgpack library.
 *
 * \param[in] object Object in msgpack library.
 * \return Response.
 */
[[nodiscard]] inline ParsedResponse parse_response_from_object(
    msgpack::object_handle object) {
    constexpr std::uint32_t num_elements_in_root_array = 4;
    if (object->via.array.size != num_elements_in_root_array) {
        throw MsgpackRPCException(StatusCode::INVALID_MESSAGE,
            "Invalid size of the array of a message.");
    }

    const auto message_id =
        parse_message_id_from_object(object->via.array.ptr[1]);
    const bool has_error = !(object->via.array.ptr[2].is_nil());
    if (has_error) {
        return ParsedResponse(message_id,
            CallResult::create_error(
                object->via.array.ptr[2], std::move(object.zone())));
    }
    return ParsedResponse(message_id,
        CallResult::create_result(
            object->via.array.ptr[3], std::move(object.zone())));
}

/*!
 * \brief Parse a notification from an object in msgpack library.
 *
 * \param[in] object Object in msgpack library.
 * \return Notification.
 */
[[nodiscard]] inline ParsedNotification parse_notification_from_object(
    msgpack::object_handle object) {
    constexpr std::uint32_t num_elements_in_root_array = 3;
    if (object->via.array.size != num_elements_in_root_array) {
        throw MsgpackRPCException(StatusCode::INVALID_MESSAGE,
            "Invalid size of the array of a message.");
    }

    auto method_name = parse_method_name_from_object(object->via.array.ptr[1]);
    auto parameters =
        ParsedParameters(object->via.array.ptr[2], std::move(object.zone()));
    return ParsedNotification(method_name, std::move(parameters));
}

/*!
 * \brief Parse a message from an object in msgpack library.
 *
 * \param[in] object Object in msgpack library.
 * \return Message.
 */
[[nodiscard]] inline ParsedMessage parse_message_from_object(
    msgpack::object_handle object) {
    if (object->type != msgpack::type::ARRAY) {
        throw MsgpackRPCException(
            StatusCode::INVALID_MESSAGE, "Invalid type of a message.");
    }
    constexpr std::uint32_t min_elements_in_root_array = 3;
    if (object->via.array.size < min_elements_in_root_array) {
        throw MsgpackRPCException(StatusCode::INVALID_MESSAGE,
            "Invalid size of the array of a message.");
    }

    const auto message_type =
        parse_message_type_from_object(object->via.array.ptr[0]);
    switch (message_type) {
    case MessageType::REQUEST:
        return parse_request_from_object(std::move(object));
    case MessageType::RESPONSE:
        return parse_response_from_object(std::move(object));
    case MessageType::NOTIFICATION:
        return parse_notification_from_object(std::move(object));
    }
    // This line won't be executed without a bug.
    std::abort();
}

}  // namespace msgpack_rpc::messages::impl
