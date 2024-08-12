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
 * \brief Definition of ConnectionWrapper class.
 */
#pragma once

#include <memory>
#include <utility>

#include "msgpack_rpc/addresses/i_address.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class to wrap IConnection objects with template functions.
 */
class ConnectionWrapper {
public:
    /*!
     * \brief Type of callback functions called when a message is received.
     *
     * Parameters:
     *
     * 1. Received message.
     */
    using MessageReceivedCallback = IConnection::MessageReceivedCallback;

    /*!
     * \brief Type of callback functions called when a message is successfully
     * sent.
     */
    using MessageSentCallback = IConnection::MessageSentCallback;

    /*!
     * \brief Type of callback functions called when a connection is closed.
     *
     * Parameters:
     *
     * 1. Status object specifying the reason.
     */
    using ConnectionClosedCallback = IConnection::ConnectionClosedCallback;

    /*!
     * \brief Constructor.
     *
     * \param[in] connection Connection.
     */
    explicit ConnectionWrapper(std::shared_ptr<IConnection> connection) noexcept
        : connection_(std::move(connection)) {}

    /*!
     * \brief Start process of this connection.
     *
     * \param[in] on_received Callback function called when a message is
     * received.
     * \param[in] on_sent Callback function called when a message is
     * sent.
     * \param[in] on_closed Callback function called when this connection is
     * closed.
     */
    void start(MessageReceivedCallback on_received, MessageSentCallback on_sent,
        ConnectionClosedCallback on_closed) {
        connection_->start(
            std::move(on_received), std::move(on_sent), std::move(on_closed));
    }

    /*!
     * \brief Asynchronously send a request.
     *
     * \tparam Parameters Types of parameters.
     * \param[in] method_name Method name.
     * \param[in] message_id Message ID.
     * \param[in] parameters Parameters.
     */
    template <typename... Parameters>
    void async_request(messages::MethodNameView method_name,
        messages::MessageID message_id, const Parameters&... parameters) {
        connection_->async_send(messages::MessageSerializer::serialize_request(
            method_name, message_id, parameters...));
    }

    /*!
     * \brief Asynchronously send a response without an error.
     *
     * \tparam T Type of the result.
     * \param[in] request_id Message ID of the request.
     * \param[in] result Result.
     */
    template <typename T>
    void async_response_success(
        messages::MessageID request_id, const T& result) {
        connection_->async_send(
            messages::MessageSerializer::serialize_successful_response(
                request_id, result));
    }

    /*!
     * \brief Asynchronously send a response with an error.
     *
     * \tparam T Type of the error.
     * \param[in] request_id Message ID of the request.
     * \param[in] error Error.
     */
    template <typename T>
    void async_response_error(messages::MessageID request_id, const T& error) {
        connection_->async_send(
            messages::MessageSerializer::serialize_error_response(
                request_id, error));
    }

    /*!
     * \brief Asynchronously send a notification.
     *
     * \tparam Parameters Types of parameters.
     * \param[in] method_name Method name.
     * \param[in] parameters Parameters.
     */
    template <typename... Parameters>
    void async_notify(
        messages::MethodNameView method_name, const Parameters&... parameters) {
        connection_->async_send(
            messages::MessageSerializer::serialize_notification(
                method_name, parameters...));
    }

    /*!
     * \brief Asynchronously close this connection.
     */
    void async_close() { connection_->async_close(); }

    /*!
     * \brief Get the address of the local endpoint.
     *
     * \return Address.
     */
    [[nodiscard]] const addresses::IAddress& local_address() const noexcept {
        return connection_->local_address();
    }

    /*!
     * \brief Get the address of the remote endpoint.
     *
     * \return Address.
     */
    [[nodiscard]] const addresses::IAddress& remote_address() const noexcept {
        return connection_->remote_address();
    }

    /*!
     * \brief Get the IConnection object of this wrapper.
     *
     * \return Pointer of the IConnection object.
     */
    [[nodiscard]] std::shared_ptr<IConnection> connection() const noexcept {
        return connection_;
    }

private:
    //! Connection.
    std::shared_ptr<IConnection> connection_;
};

}  // namespace msgpack_rpc::transport
