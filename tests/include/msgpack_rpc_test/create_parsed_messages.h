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
 * \brief Definition of functions to create objects of parsed messages.
 */
#pragma once

#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc_test/parse_messages.h"

namespace msgpack_rpc_test {

/*!
 * \brief Create an object of a parsed request.
 *
 * \tparam Parameters Types of parameters.
 * \param[in] method_name Method name.
 * \param[in] message_id Message ID.
 * \param[in] parameters Parameters.
 * \return Parsed request.
 */
template <typename... Parameters>
[[nodiscard]] inline msgpack_rpc::messages::ParsedRequest create_parsed_request(
    msgpack_rpc::messages::MethodNameView method_name,
    msgpack_rpc::messages::MessageID message_id,
    const Parameters&... parameters) {
    return parse_request(
        msgpack_rpc::messages::MessageSerializer::serialize_request(
            method_name, message_id, parameters...));
}

/*!
 * \brief Create an object of a parsed response without an error.
 *
 * \tparam T Type of the result.
 * \param[in] request_id Request ID.
 * \param[in] result Result.
 * \return Parsed response.
 */
template <typename T>
[[nodiscard]] inline msgpack_rpc::messages::ParsedResponse
create_parsed_successful_response(
    msgpack_rpc::messages::MessageID request_id, const T& result) {
    return parse_response(
        msgpack_rpc::messages::MessageSerializer::serialize_successful_response(
            request_id, result));
}

/*!
 * \brief Create an object of a parsed notification.
 *
 * \tparam Parameters Types of parameters.
 * \param[in] method_name Method name.
 * \param[in] parameters Parameters.
 * \return Parsed notification.
 */
template <typename... Parameters>
[[nodiscard]] inline msgpack_rpc::messages::ParsedNotification
create_parsed_notification(msgpack_rpc::messages::MethodNameView method_name,
    const Parameters&... parameters) {
    return parse_notification(
        msgpack_rpc::messages::MessageSerializer::serialize_notification(
            method_name, parameters...));
}

}  // namespace msgpack_rpc_test
