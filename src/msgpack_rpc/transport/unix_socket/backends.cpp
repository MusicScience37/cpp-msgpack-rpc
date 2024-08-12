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
 * \brief Implementation of functions to create backends.
 */
#include "msgpack_rpc/transport/backends.h"

#include "msgpack_rpc/impl/config.h"

#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS

#include <memory>
#include <utility>

#include "msgpack_rpc/transport/i_backend.h"
#include "msgpack_rpc/transport/unix_socket/unix_socket_backend.h"

namespace msgpack_rpc::transport {

std::shared_ptr<IBackend> create_unix_socket_backend(
    const std::shared_ptr<executors::IExecutor>& executor,
    const config::MessageParserConfig& message_parser_config,
    std::shared_ptr<logging::Logger> logger) {
    return std::make_shared<unix_socket::UnixSocketBackend>(
        executor, message_parser_config, std::move(logger));
}

}  // namespace msgpack_rpc::transport

#endif
