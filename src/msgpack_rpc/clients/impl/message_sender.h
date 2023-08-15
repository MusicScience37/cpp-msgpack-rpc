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
 * \brief Definition of MessageSender class.
 */
#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include "msgpack_rpc/clients/impl/client_connector.h"
#include "msgpack_rpc/clients/impl/sent_message_queue.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class to send messages in clients.
 */
class MessageSender {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] connector Connector.
     * \param[in] logger Logger.
     */
    MessageSender(std::weak_ptr<ClientConnector> connector,
        std::shared_ptr<logging::Logger> logger)
        : connector_(std::move(connector)), logger_(std::move(logger)) {}

    /*!
     * \brief Send a message.
     *
     * \param[in] message Message.
     * \param[in] id Message ID (for requests).
     */
    void send(std::shared_ptr<messages::SerializedMessage> message,
        std::optional<messages::MessageID> id = std::nullopt) {
        sent_messages_.push(std::move(message), id);
        send_next();
    }

    /*!
     * \brief Send the next message if possible.
     */
    void send_next() {
        if (is_sending_.load(std::memory_order_relaxed)) {
            MSGPACK_RPC_TRACE(logger_, "Another message is being sent.");
            return;
        }
        const auto connection = get_connection();
        if (!connection) {
            MSGPACK_RPC_TRACE(
                logger_, "No connection now, so wait for connection.");
            return;
        }
        const auto [message, request_id] = sent_messages_.next();
        if (!message) {
            MSGPACK_RPC_TRACE(logger_, "No message to be sent for now.");
            return;
        }
        if (is_sending_.exchange(true, std::memory_order_acquire)) {
            MSGPACK_RPC_TRACE(logger_, "Another message is being sent.");
            return;
        }

        connection->async_send(message);
        MSGPACK_RPC_TRACE(logger_, "Sending next message.");
    }

    /*!
     * \brief Handle a sent message.
     */
    void handle_sent_message() {
        MSGPACK_RPC_TRACE(logger_, "A message has been sent.");
        sent_messages_.pop();
        is_sending_.store(false, std::memory_order_release);
    }

    /*!
     * \brief Handle disconnection.
     */
    void handle_disconnection() {
        MSGPACK_RPC_TRACE(logger_, "Connection closed, so reconnecting.");
        is_sending_.store(false, std::memory_order_release);
    }

private:
    /*!
     * \brief Get the connection.
     *
     * \return Connection if exists.
     */
    [[nodiscard]] std::shared_ptr<transport::IConnection> get_connection() {
        const auto connector = connector_.lock();
        if (!connector) {
            return nullptr;
        }
        return connector->connection();
    }

    //! Connector.
    std::weak_ptr<ClientConnector> connector_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Queue of messages to be sent.
    SentMessageQueue sent_messages_{};

    //! Whether this client is sending a message.
    std::atomic<bool> is_sending_{false};
};

}  // namespace msgpack_rpc::clients::impl
