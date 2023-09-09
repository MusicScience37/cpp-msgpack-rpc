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
#include <string>
#include <string_view>

#include <toml++/toml.h>

#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/config/reconnection_config.h"
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

}  // namespace msgpack_rpc::config::toml::impl
