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
 * \brief Implementation of TCPBackend class.
 */
#include "msgpack_rpc/transport/tcp/tcp_backend.h"

#include <memory>
#include <utility>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/transport/ip_acceptor_factory.h"
#include "msgpack_rpc/transport/ip_connector.h"
#include "msgpack_rpc/transport/tcp/tcp_acceptor_factory.h"
#include "msgpack_rpc/transport/tcp/tcp_connector.h"

namespace msgpack_rpc::transport::tcp {

TCPBackend::TCPBackend(const std::shared_ptr<executors::IExecutor>& executor,
    const config::MessageParserConfig& message_parser_config,
    std::shared_ptr<logging::Logger> logger)
    : executor_(executor),
      message_parser_config_(message_parser_config),
      logger_(std::move(logger)) {}

std::string_view TCPBackend::scheme() const noexcept {
    return addresses::TCP_SCHEME;
}

std::shared_ptr<IAcceptorFactory> TCPBackend::create_acceptor_factory() {
    return std::make_shared<TCPAcceptorFactory>(
        executor(), message_parser_config_, logger_);
}

std::shared_ptr<IConnector> TCPBackend::create_connector() {
    return std::make_shared<TCPConnector>(
        executor(), message_parser_config_, logger_);
}

TCPBackend::~TCPBackend() noexcept = default;

std::shared_ptr<executors::IExecutor> TCPBackend::executor() const {
    auto locked = executor_.lock();
    if (!locked) {
        throw MsgpackRPCException(
            StatusCode::PRECONDITION_NOT_MET, "Executor has been deleted.");
    }
    return locked;
}

}  // namespace msgpack_rpc::transport::tcp
