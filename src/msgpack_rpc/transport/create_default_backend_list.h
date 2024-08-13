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
 * \brief Definition of create_default_backend_list function.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/backends.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Create a list of backends with default backends of protocols.
 *
 * \param[in] executor Executor.
 * \param[in] message_parser_config Configuration of parsers of messages.
 * \param[in] logger Logger.
 * \return List of backends.
 */
[[nodiscard]] inline BackendList create_default_backend_list(
    const std::shared_ptr<executors::IExecutor> &executor,
    const config::MessageParserConfig &message_parser_config,
    const std::shared_ptr<logging::Logger> &logger) {
    BackendList backends;
    backends.append(
        create_tcp_backend(executor, message_parser_config, logger));
#if MSGPACK_RPC_HAS_UNIX_SOCKETS
    backends.append(
        create_unix_socket_backend(executor, message_parser_config, logger));
#endif
    return backends;
}

}  // namespace msgpack_rpc::transport
