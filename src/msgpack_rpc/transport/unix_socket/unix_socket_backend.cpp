/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of UnixSocketBackend class.
 */
#include "msgpack_rpc/transport/unix_socket/unix_socket_backend.h"

#include "msgpack_rpc/impl/config.h"

#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS

#include <memory>
#include <utility>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/unix_socket/unix_socket_acceptor_factory.h"
#include "msgpack_rpc/transport/unix_socket/unix_socket_connector.h"

namespace msgpack_rpc::transport::unix_socket {

UnixSocketBackend::UnixSocketBackend(
    const std::shared_ptr<executors::IExecutor>& executor,
    const config::MessageParserConfig& message_parser_config,
    std::shared_ptr<logging::Logger> logger)
    : executor_(executor),
      message_parser_config_(message_parser_config),
      logger_(std::move(logger)) {}

std::string_view UnixSocketBackend::scheme() const noexcept {
    return addresses::UNIX_SOCKET_SCHEME;
}

std::shared_ptr<IAcceptorFactory> UnixSocketBackend::create_acceptor_factory() {
    return std::make_shared<UnixSocketAcceptorFactory>(executor(),
        message_parser_config_, logger_, addresses::UNIX_SOCKET_SCHEME);
}

std::shared_ptr<IConnector> UnixSocketBackend::create_connector() {
    return std::make_shared<UnixSocketConnector>(executor(),
        message_parser_config_, logger_, addresses::UNIX_SOCKET_SCHEME);
}

UnixSocketBackend::~UnixSocketBackend() noexcept = default;

std::shared_ptr<executors::IExecutor> UnixSocketBackend::executor() const {
    auto locked = executor_.lock();
    if (!locked) {
        throw MsgpackRPCException(
            StatusCode::PRECONDITION_NOT_MET, "Executor has been deleted.");
    }
    return locked;
}

}  // namespace msgpack_rpc::transport::unix_socket

#endif
