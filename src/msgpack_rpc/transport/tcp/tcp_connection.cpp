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
 * \brief Implementation of TCPConnection class.
 */
#if false

#include "msgpack_rpc/transport/tcp/tcp_connection.h"

#include <utility>

#include "msgpack_rpc/addresses/tcp_address.h"

namespace msgpack_rpc::transport::tcp {

TCPConnection::TCPConnection(std::shared_ptr<StreamConnectionImpl> connection)
    : connection_(std::move(connection)),
      local_address_(
          addresses::TCPAddress(connection_->socket().local_endpoint())),
      remote_address_(
          addresses::TCPAddress(connection_->socket().remote_endpoint())) {}

void TCPConnection::start(MessageReceivedCallback on_received,
    MessageSentCallback on_sent, ConnectionClosedCallback on_closed) {
    std::string log_name = fmt::format(
        "Connection(local={}, remote={})", local_address_, remote_address_);
    connection_->start(std::move(on_received), std::move(on_sent),
        std::move(on_closed), std::move(log_name));
}

void TCPConnection::async_send(
    std::shared_ptr<messages::SerializedMessage> message) {
    connection_->async_send(std::move(message));
}

void TCPConnection::async_close() { connection_->async_close(); }

const addresses::Address& TCPConnection::local_address() const noexcept {
    return local_address_;
}

const addresses::Address& TCPConnection::remote_address() const noexcept {
    return remote_address_;
}

TCPConnection::~TCPConnection() = default;

}  // namespace msgpack_rpc::transport::tcp

#endif
