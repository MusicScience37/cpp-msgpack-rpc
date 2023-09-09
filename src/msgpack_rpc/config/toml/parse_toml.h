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
 * \brief Definition of parse_toml functions.
 */
#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::config::toml {

/*!
 * \brief Parse configurations from a TOML file.
 *
 * \param[in] filepath Path of the TOML file.
 * \param[in] logging_configs Configurations of logging.
 * \param[in] client_configs Configurations of clients.
 * \param[in] server_configs Configurations of servers.
 */
MSGPACK_RPC_EXPORT void parse_toml(std::string_view filepath,
    std::unordered_map<std::string, LoggingConfig>& logging_configs,
    std::unordered_map<std::string, ClientConfig>& client_configs,
    std::unordered_map<std::string, ServerConfig>& server_configs);

}  // namespace msgpack_rpc::config::toml
