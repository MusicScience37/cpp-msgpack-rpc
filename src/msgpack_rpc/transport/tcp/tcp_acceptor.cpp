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
 * \brief Implementation of TCPAcceptor class.
 */
#if false

#include "msgpack_rpc/transport/tcp/tcp_acceptor.h"

#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "msgpack_rpc/transport/i_connection.h"
#include "msgpack_rpc/transport/stream_connection_impl.h"
#include "msgpack_rpc/transport/tcp/tcp_connection.h"

namespace msgpack_rpc::transport::tcp {

TCPAcceptor::TCPAcceptor(std::shared_ptr<AcceptorImpl> acceptor)
    : acceptor_(std::move(acceptor)),
      local_address_(
          addresses::TCPAddress(acceptor_->acceptor().local_endpoint())) {}

void TCPAcceptor::start(ConnectionCallback on_connection) {
    std::string log_name = fmt::format("Acceptor(local={})", local_address_);
    acceptor_->start(
        [on_connection](std::shared_ptr<StreamConnectionImpl> connection_impl) {
            std::shared_ptr<IConnection> connection =
                std::make_shared<TCPConnection>(std::move(connection_impl));
            on_connection(std::move(connection));
        },
        std::move(log_name));
}

void TCPAcceptor::stop() { acceptor_->async_stop(); }

const addresses::Address& TCPAcceptor::local_address() const noexcept {
    return local_address_;
}

TCPAcceptor::~TCPAcceptor() = default;

}  // namespace msgpack_rpc::transport::tcp

#endif
