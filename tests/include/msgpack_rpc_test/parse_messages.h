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
 * \brief Definition of functions to parse messages.
 */
#pragma once

#include <variant>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

#include "msgpack_rpc/messages/impl/parse_message_from_object.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc_test {

/*!
 * \brief Parse a message.
 *
 * \param[in] message Serialized message.
 * \return Message.
 */
[[nodiscard]] inline msgpack_rpc::messages::ParsedMessage parse_message(
    const msgpack_rpc::messages::SerializedMessage& message) {
    return msgpack_rpc::messages::impl::parse_message_from_object(
        msgpack::unpack(message.data(), message.size()));
}

/*!
 * \brief Parse a request.
 *
 * \param[in] message Serialized message.
 * \return Request.
 */
[[nodiscard]] inline msgpack_rpc::messages::ParsedRequest parse_request(
    const msgpack_rpc::messages::SerializedMessage& message) {
    auto parsed_message = parse_message(message);
    REQUIRE(std::holds_alternative<msgpack_rpc::messages::ParsedRequest>(
        parsed_message));
    return msgpack_rpc::messages::ParsedRequest(std::move(
        std::get<msgpack_rpc::messages::ParsedRequest>(parsed_message)));
}

/*!
 * \brief Parse a response.
 *
 * \param[in] message Serialized message.
 * \return Response.
 */
[[nodiscard]] inline msgpack_rpc::messages::ParsedResponse parse_response(
    const msgpack_rpc::messages::SerializedMessage& message) {
    auto parsed_message = parse_message(message);
    REQUIRE(std::holds_alternative<msgpack_rpc::messages::ParsedResponse>(
        parsed_message));
    return msgpack_rpc::messages::ParsedResponse(std::move(
        std::get<msgpack_rpc::messages::ParsedResponse>(parsed_message)));
}

/*!
 * \brief Parse a notification.
 *
 * \param[in] message Serialized message.
 * \return Notification.
 */
[[nodiscard]] inline msgpack_rpc::messages::ParsedNotification
parse_notification(const msgpack_rpc::messages::SerializedMessage& message) {
    auto parsed_message = parse_message(message);
    REQUIRE(std::holds_alternative<msgpack_rpc::messages::ParsedNotification>(
        parsed_message));
    return msgpack_rpc::messages::ParsedNotification(std::move(
        std::get<msgpack_rpc::messages::ParsedNotification>(parsed_message)));
}

}  // namespace msgpack_rpc_test
