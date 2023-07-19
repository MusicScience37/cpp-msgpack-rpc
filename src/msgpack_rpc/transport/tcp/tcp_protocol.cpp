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
 * \brief Implementation of TCPProtocol class.
 */
#include "msgpack_rpc/transport/tcp/tcp_protocol.h"

#include <memory>
#include <utility>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/transport/tcp/tcp_acceptor.h"
#include "msgpack_rpc/transport/tcp/tcp_connector.h"
#include "msgpack_rpc/transport/tcp/tcp_resolver.h"

namespace msgpack_rpc::transport::tcp {

TCPProtocol::TCPProtocol(const std::shared_ptr<executors::IExecutor>& executor,
    const config::MessageParserConfig& message_parser_config,
    std::shared_ptr<logging::Logger> logger)
    : executor_(executor),
      message_parser_config_(message_parser_config),
      logger_(std::move(logger)) {}

std::string_view TCPProtocol::scheme() const noexcept {
    return addresses::TCP_SCHEME;
}

std::shared_ptr<IAcceptor> TCPProtocol::create_acceptor(
    const addresses::Address& local_address) {
    return std::make_shared<TCPAcceptor>(
        local_address.as_tcp(), executor(), message_parser_config_, logger_);
}

std::shared_ptr<IConnector> TCPProtocol::create_connector() {
    return std::make_shared<TCPConnector>(
        executor(), message_parser_config_, logger_);
}

std::shared_ptr<IResolver> TCPProtocol::create_resolver() {
    return std::make_shared<TCPResolver>(executor(), logger_);
}

TCPProtocol::~TCPProtocol() noexcept = default;

std::shared_ptr<executors::IExecutor> TCPProtocol::executor() const {
    auto locked = executor_.lock();
    if (!locked) {
        throw MsgpackRPCException(
            StatusCode::PRECONDITION_NOT_MET, "Executor has been deleted.");
    }
    return locked;
}

}  // namespace msgpack_rpc::transport::tcp
