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
 * \brief Definition of TCPConnection class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "msgpack_rpc/transport/stream_connection_impl.h"

namespace msgpack_rpc::transport::tcp {

/*!
 * \brief Class of connections of TCP.
 */
class TCPConnection final : public IConnection {
public:
    /*!
     * \brief Connection.
     *
     * \param[in] connection
     */
    explicit TCPConnection(std::shared_ptr<StreamConnectionImpl> connection);

    //! \copydoc msgpack_rpc::transport::IConnection::start
    void start(MessageReceivedCallback on_received, MessageSentCallback on_sent,
        ConnectionClosedCallback on_closed) override;

    //! \copydoc msgpack_rpc::transport::IConnection::async_send
    void async_send(
        std::shared_ptr<messages::SerializedMessage> message) override;

    //! \copydoc msgpack_rpc::transport::IConnection::async_close
    void async_close() override;

    //! \copydoc msgpack_rpc::transport::IConnection::local_address
    [[nodiscard]] const addresses::Address& local_address()
        const noexcept override;

    //! \copydoc msgpack_rpc::transport::IConnection::remote_address
    [[nodiscard]] const addresses::Address& remote_address()
        const noexcept override;

    TCPConnection(const TCPConnection&) = delete;
    TCPConnection(TCPConnection&&) = delete;
    TCPConnection& operator=(const TCPConnection&) = delete;
    TCPConnection& operator=(TCPConnection&&) = delete;

    //! Destructor.
    ~TCPConnection() override;

private:
    //! Connection.
    std::shared_ptr<StreamConnectionImpl> connection_;

    //! Address of the local endpoint.
    addresses::Address local_address_;

    //! Address of the remote endpoint.
    addresses::Address remote_address_;
};

}  // namespace msgpack_rpc::transport::tcp
