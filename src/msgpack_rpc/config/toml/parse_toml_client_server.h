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
 * \brief Definition of parse_toml functions for clients and servers.
 */
#pragma once

#include <cstddef>
#include <exception>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <toml++/toml.h>

#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/config/reconnection_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/config/toml/parse_toml_common.h"

namespace msgpack_rpc::config::toml::impl {

/*!
 * \brief Parse a configuration of parsers of messages from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] config Configuration.
 */
inline void parse_toml(
    const ::toml::table& table, MessageParserConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "read_buffer_size") {
            MSGPACK_RPC_PARSE_TOML_VALUE(
                "read_buffer_size", read_buffer_size, std::size_t);
        }
    }
}

/*!
 * \brief Parse a configuration of executors from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] config Configuration.
 */
inline void parse_toml(const ::toml::table& table, ExecutorConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "num_transport_threads") {
            MSGPACK_RPC_PARSE_TOML_VALUE(
                "num_transport_threads", num_transport_threads, std::size_t);
        } else if (key_str == "num_callback_threads") {
            MSGPACK_RPC_PARSE_TOML_VALUE(
                "num_callback_threads", num_callback_threads, std::size_t);
        }
    }
}

/*!
 * \brief Parse a configuration of reconnection in clients from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] config Configuration.
 */
inline void parse_toml(const ::toml::table& table, ReconnectionConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "initial_waiting_time_sec") {
            MSGPACK_RPC_PARSE_TOML_VALUE_DURATION(
                "initial_waiting_time_sec", initial_waiting_time);
        } else if (key_str == "max_waiting_time_sec") {
            MSGPACK_RPC_PARSE_TOML_VALUE_DURATION(
                "max_waiting_time_sec", max_waiting_time);
        } else if (key_str == "max_jitter_waiting_time_sec") {
            MSGPACK_RPC_PARSE_TOML_VALUE_DURATION(
                "max_jitter_waiting_time_sec", max_jitter_waiting_time);
        }
    }
}

/*!
 * \brief Parse a configuration of clients from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] config Configuration.
 */
inline void parse_toml(const ::toml::table& table, ClientConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "uris") {
            const auto* uris_node = value.as_array();
            if (uris_node == nullptr) {
                throw_error(value.source(), "uris");
            }
            for (const auto& elem : *uris_node) {
                const auto* uri_node = elem.as_string();
                if (uri_node == nullptr) {
                    throw_error(elem.source(), "uris");
                }
                try {
                    config.add_uri(uri_node->get());
                } catch (const std::exception& e) {
                    throw_error(elem.source(), "uris", e.what());
                }
            }
        } else if (key_str == "call_timeout_sec") {
            MSGPACK_RPC_PARSE_TOML_VALUE_DURATION(
                "call_timeout_sec", call_timeout);
        } else if (key_str == "message_parser") {
            const auto* child_table = value.as_table();
            if (child_table == nullptr) {
                throw_error(value.source(), "message_parser");
            }
            parse_toml(*child_table, config.message_parser());
        } else if (key_str == "executor") {
            const auto* child_table = value.as_table();
            if (child_table == nullptr) {
                throw_error(value.source(), "executor");
            }
            parse_toml(*child_table, config.executor());
        } else if (key_str == "reconnection") {
            const auto* child_table = value.as_table();
            if (child_table == nullptr) {
                throw_error(value.source(), "reconnection");
            }
            parse_toml(*child_table, config.reconnection());
        }
    }
}

/*!
 * \brief Parse configurations of clients from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] configs Configurations.
 */
inline void parse_toml(const ::toml::table& table,
    std::unordered_map<std::string, ClientConfig>& configs) {
    for (const auto& [key, value] : table) {
        const auto* table_ptr = value.as_table();
        if (table_ptr == nullptr) {
            throw_error(value.source(), "client",
                "\"client\" must be a table of tables.");
        }
        ClientConfig config;
        parse_toml(*table_ptr, config);
        configs.try_emplace(std::string(key.str()), std::move(config));
    }
}

/*!
 * \brief Parse a configuration of servers from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] config Configuration.
 */
inline void parse_toml(const ::toml::table& table, ServerConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "uris") {
            const auto* uris_node = value.as_array();
            if (uris_node == nullptr) {
                throw_error(value.source(), "uris");
            }
            for (const auto& elem : *uris_node) {
                const auto* uri_node = elem.as_string();
                if (uri_node == nullptr) {
                    throw_error(elem.source(), "uris");
                }
                try {
                    config.add_uri(uri_node->get());
                } catch (const std::exception& e) {
                    throw_error(elem.source(), "uris", e.what());
                }
            }
        } else if (key_str == "message_parser") {
            const auto* child_table = value.as_table();
            if (child_table == nullptr) {
                throw_error(value.source(), "message_parser");
            }
            parse_toml(*child_table, config.message_parser());
        } else if (key_str == "executor") {
            const auto* child_table = value.as_table();
            if (child_table == nullptr) {
                throw_error(value.source(), "executor");
            }
            parse_toml(*child_table, config.executor());
        }
    }
}

/*!
 * \brief Parse configurations of servers from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] configs Configurations.
 */
inline void parse_toml(const ::toml::table& table,
    std::unordered_map<std::string, ServerConfig>& configs) {
    for (const auto& [key, value] : table) {
        const auto* table_ptr = value.as_table();
        if (table_ptr == nullptr) {
            throw_error(value.source(), "server",
                "\"server\" must be a table of tables.");
        }
        ServerConfig config;
        parse_toml(*table_ptr, config);
        configs.try_emplace(std::string(key.str()), std::move(config));
    }
}

}  // namespace msgpack_rpc::config::toml::impl
