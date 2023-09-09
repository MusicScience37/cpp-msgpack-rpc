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
 * \brief Implementation of parse_toml functions.
 */
#pragma once

#include <string>
#include <unordered_map>

#include <toml++/toml.h>

#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/config/toml/parse_toml_client_server.h"
#include "msgpack_rpc/config/toml/parse_toml_common.h"
#include "msgpack_rpc/config/toml/parse_toml_logging.h"

namespace msgpack_rpc::config::toml::impl {

/*!
 * \brief Parse configurations from TOML.
 *
 * \param[in] root_table Root table of TOML file.
 * \param[in] logging_configs Configurations of logging.
 * \param[in] client_configs Configurations of clients.
 * \param[in] server_configs Configurations of servers.
 */
inline void parse_toml(const ::toml::table& root_table,
    std::unordered_map<std::string, LoggingConfig>& logging_configs,
    std::unordered_map<std::string, ClientConfig>& client_configs,
    std::unordered_map<std::string, ServerConfig>& server_configs) {
    if (const auto logging_node = root_table.at_path("logging")) {
        const auto* logging_table = logging_node.as_table();
        if (logging_table == nullptr) {
            impl::throw_error(logging_node.node()->source(), "logging",
                "\"logging\" must be a table of tables.");
        }
        impl::parse_toml(*logging_table, logging_configs);
    }

    if (const auto client_node = root_table.at_path("client")) {
        const auto* client_table = client_node.as_table();
        if (client_table == nullptr) {
            impl::throw_error(client_node.node()->source(), "client",
                "\"client\" must be a table of tables.");
        }
        impl::parse_toml(*client_table, client_configs);
    }

    if (const auto server_node = root_table.at_path("server")) {
        const auto* server_table = server_node.as_table();
        if (server_table == nullptr) {
            impl::throw_error(server_node.node()->source(), "server",
                "\"server\" must be a table of tables.");
        }
        impl::parse_toml(*server_table, server_configs);
    }
}

}  // namespace msgpack_rpc::config::toml::impl
