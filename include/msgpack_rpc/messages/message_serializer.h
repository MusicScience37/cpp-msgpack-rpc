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
 * \brief Definition of MessageSerializer class.
 */
#pragma once

#include <tuple>

#include <msgpack.hpp>

#include "msgpack_rpc/messages/impl/serialization_buffer.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class to serialize messages.
 */
class MessageSerializer {
public:
    /*!
     * \brief Serialize a request.
     *
     * \tparam Parameters Types of parameters.
     * \param[in] method_name Method name.
     * \param[in] message_id Message ID.
     * \param[in] parameters Parameters.
     * \return Serialized data.
     */
    template <typename... Parameters>
    [[nodiscard]] static SerializedMessage serialize_request(
        MethodNameView method_name, MessageID message_id,
        const Parameters&... parameters) {
        impl::SerializationBuffer buffer;
        msgpack::packer<impl::SerializationBuffer> packer{buffer};
        packer.pack_array(4);
        packer.pack(0);
        packer.pack(message_id);
        packer.pack(method_name.name());
        packer.pack(std::forward_as_tuple(parameters...));
        return buffer.release();
    }

    /*!
     * \brief Serialize a successful response.
     *
     * \tparam T Type of the result.
     * \param[in] request_id Message ID of the request.
     * \param[in] result Result.
     * \return Serialized data.
     */
    template <typename T>
    [[nodiscard]] static SerializedMessage serialize_successful_response(
        MessageID request_id, const T& result) {
        impl::SerializationBuffer buffer;
        msgpack::packer<impl::SerializationBuffer> packer{buffer};
        packer.pack_array(4);
        packer.pack(1);
        packer.pack(request_id);
        packer.pack_nil();
        packer.pack(result);
        return buffer.release();
    }

    /*!
     * \brief Serialize an error response.
     *
     * \tparam T Type of the error.
     * \param[in] request_id Message ID of the request.
     * \param[in] error Error.
     * \return Serialized data.
     */
    template <typename T>
    [[nodiscard]] static SerializedMessage serialize_error_response(
        MessageID request_id, const T& error) {
        impl::SerializationBuffer buffer;
        msgpack::packer<impl::SerializationBuffer> packer{buffer};
        packer.pack_array(4);
        packer.pack(1);
        packer.pack(request_id);
        packer.pack(error);
        packer.pack_nil();
        return buffer.release();
    }

    /*!
     * \brief Serialize a notification.
     *
     * \tparam Parameters Types of parameters.
     * \param[in] method_name Method name.
     * \param[in] parameters Parameters.
     * \return Serialized data.
     */
    template <typename... Parameters>
    [[nodiscard]] static SerializedMessage serialize_notification(
        MethodNameView method_name, const Parameters&... parameters) {
        impl::SerializationBuffer buffer;
        msgpack::packer<impl::SerializationBuffer> packer{buffer};
        packer.pack_array(3);
        packer.pack(2);
        packer.pack(method_name.name());
        packer.pack(std::forward_as_tuple(parameters...));
        return buffer.release();
    }
};

}  // namespace msgpack_rpc::messages
