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
 * \brief Definition of IConnection class.
 */
#pragma once

#include <functional>
#include <memory>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Interface of connections.
 */
class IConnection {
public:
    /*!
     * \brief Type of callback functions called when a message is received.
     *
     * Parameters:
     *
     * 1. Received message.
     */
    using MessageReceivedCallback =
        std::function<void(messages::ParsedMessage)>;

    /*!
     * \brief Type of callback functions called when a message is successfully
     * sent.
     */
    using MessageSentCallback = std::function<void()>;

    /*!
     * \brief Type of callback functions called when a connection is closed.
     *
     * Parameters:
     *
     * 1. Status object specifying the reason.
     */
    using ConnectionClosedCallback = std::function<void(Status)>;

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
    virtual void start(MessageReceivedCallback on_received,
        MessageSentCallback on_sent, ConnectionClosedCallback on_closed) = 0;

    /*!
     * \brief Asynchronously send a message.
     *
     * \param[in] message Message to send.
     */
    virtual void async_send(
        std::shared_ptr<messages::SerializedMessage> message) = 0;

    /*!
     * \brief Asynchronously close this connection.
     */
    virtual void async_close() = 0;

    /*!
     * \brief Get the address of the local endpoint.
     *
     * \return Address.
     */
    [[nodiscard]] virtual const addresses::Address& local_address()
        const noexcept = 0;

    /*!
     * \brief Get the address of the remote endpoint.
     *
     * \return Address.
     */
    [[nodiscard]] virtual const addresses::Address& remote_address()
        const noexcept = 0;

    IConnection(const IConnection&) = delete;
    IConnection(IConnection&&) = delete;
    IConnection& operator=(const IConnection&) = delete;
    IConnection& operator=(IConnection&&) = delete;

    //! Destructor.
    virtual ~IConnection() noexcept = default;

protected:
    //! Constructor.
    IConnection() noexcept = default;
};

}  // namespace msgpack_rpc::transport
