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
 * \brief Declaration of functions to create backends.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/i_backend.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Create a backend of TCP.
 *
 * \param[in] executor Executor.
 * \param[in] message_parser_config Configuration of parsers of messages.
 * \param[in] logger Logger.
 * \return Backend.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<IBackend> create_tcp_backend(
    const std::shared_ptr<executors::IExecutor>& executor,
    const config::MessageParserConfig& message_parser_config,
    std::shared_ptr<logging::Logger> logger);

#if MSGPACK_RPC_HAS_UNIX_SOCKETS

/*!
 * \brief Create a backend of Unix sockets.
 *
 * \param[in] executor Executor.
 * \param[in] message_parser_config Configuration of parsers of messages.
 * \param[in] logger Logger.
 * \return Backend.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<IBackend>
create_unix_socket_backend(
    const std::shared_ptr<executors::IExecutor>& executor,
    const config::MessageParserConfig& message_parser_config,
    std::shared_ptr<logging::Logger> logger);

#endif

}  // namespace msgpack_rpc::transport
