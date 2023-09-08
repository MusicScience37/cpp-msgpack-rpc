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

#include <string_view>
#include <unordered_map>

#include <fmt/format.h>
#include <toml++/impl/source_region.h>
#include <toml++/toml.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/log_level.h"

namespace msgpack_rpc::config::toml {

namespace impl {

/*!
 * \brief Throw an exception for an error of TOML.
 *
 * \param[in] source Location in TOML.
 * \param[in] config_key Key of the configuration.
 */
[[noreturn]] void throw_error(
    const ::toml::source_region& source, std::string_view config_key) {
    const std::string_view source_path =
        (source.path) ? *source.path : std::string_view("(unknown file)");
    throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
        fmt::format("Invalid value for {}. (at {}:{})", config_key, source_path,
            source.begin.line));
}

/*!
 * \brief Throw an exception for an error of TOML.
 *
 * \param[in] source Location in TOML.
 * \param[in] config_key Key of the configuration.
 * \param[in] error_message Error message.
 */
[[noreturn]] void throw_error(const ::toml::source_region& source,
    std::string_view config_key, std::string_view error_message) {
    const std::string_view source_path =
        (source.path) ? *source.path : std::string_view("(unknown file)");
    throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
        fmt::format("Invalid value for {}: {} (at {}:{})", config_key,
            error_message, source_path, source.begin.line));
}

//! Internal macro to parse a value from TOML.
#define MSGPACK_RPC_PARSE_TOML_VALUE(KEY_STR, CONFIG_FUNCTION, TYPE) \
    const auto config_value = value.value<TYPE>();                   \
    if (!config_value) {                                             \
        throw_error(value.source(), KEY_STR);                        \
    }                                                                \
    try {                                                            \
        config.CONFIG_FUNCTION(*config_value);                       \
    } catch (const std::exception& e) {                              \
        throw_error(value.source(), KEY_STR, e.what());              \
    }

/*!
 * \brief Parse a log level from a string.
 *
 * \param[in] str String.
 * \param[in] source Location in TOML. (For errors.)
 * \param[in] config_key Key of the configuration.
 * \return Log level.
 */
[[nodiscard]] logging::LogLevel parse_log_level(std::string_view str,
    const ::toml::source_region& source, std::string_view config_key) {
    if (str == "trace") {
        return logging::LogLevel::TRACE;
    }
    if (str == "debug") {
        return logging::LogLevel::DEBUG;
    }
    if (str == "info") {
        return logging::LogLevel::INFO;
    }
    if (str == "warn") {
        return logging::LogLevel::WARN;
    }
    if (str == "error") {
        return logging::LogLevel::ERROR;
    }
    if (str == "critical") {
        return logging::LogLevel::CRITICAL;
    }
    throw_error(source, config_key);
}

/*!
 * \brief Parse a configuration of logging from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] config Configuration.
 */
void parse_toml(const ::toml::table& table, LoggingConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "filepath") {
            MSGPACK_RPC_PARSE_TOML_VALUE("filepath", filepath, std::string);
        } else if (key_str == "max_file_size") {
            MSGPACK_RPC_PARSE_TOML_VALUE(
                "max_file_size", max_file_size, std::size_t);
        } else if (key_str == "max_files") {
            MSGPACK_RPC_PARSE_TOML_VALUE("max_files", max_files, std::size_t);
        } else if (key_str == "output_log_level") {
            const auto config_value = value.value<std::string>();
            if (!config_value) {
                throw_error(value.source(), "output_log_level");
            }
            config.output_log_level(parse_log_level(
                *config_value, value.source(), "output_log_level"));
        }
    }
}

/*!
 * \brief Parse configurations of logging from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] configs Configurations.
 */
void parse_toml(const ::toml::table& table,
    std::unordered_map<std::string, LoggingConfig>& configs) {
    for (const auto& [key, value] : table) {
        const auto* table_ptr = value.as_table();
        if (table_ptr == nullptr) {
            throw_error(value.source(), "logging",
                "\"logging\" must be a table of tables.");
        }
        LoggingConfig config;
        parse_toml(*table_ptr, config);
        configs.try_emplace(std::string(key.str()), std::move(config));
    }
}

/*!
 * \brief Parse a configuration of parsers of messages from TOML.
 *
 * \param[in] table Table in TOML.
 * \param[out] config Configuration.
 */
void parse_toml(const ::toml::table& table, MessageParserConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "read_buffer_size") {
            MSGPACK_RPC_PARSE_TOML_VALUE(
                "read_buffer_size", read_buffer_size, std::size_t);
        }
    }
}

/*!
 * \brief Parse configurations from TOML.
 *
 * \param[in] root_table Root table of TOML file.
 * \param[in] logging_configs Configurations of logging.
 * \param[in] client_configs Configurations of clients.
 * \param[in] server_configs Configurations of servers.
 */
void parse_toml(const ::toml::table& root_table,
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

    // TODO
    (void)client_configs;
    (void)server_configs;
}

}  // namespace impl

/*!
 * \brief Parse configurations from a TOML file.
 *
 * \param[in] filepath Path of the TOML file.
 * \param[in] logging_configs Configurations of logging.
 * \param[in] client_configs Configurations of clients.
 * \param[in] server_configs Configurations of servers.
 */
void parse_toml(std::string_view filepath,
    std::unordered_map<std::string, LoggingConfig>& logging_configs,
    std::unordered_map<std::string, ClientConfig>& client_configs,
    std::unordered_map<std::string, ServerConfig>& server_configs) {
    try {
        const auto root_table = ::toml::parse_file(filepath);
        impl::parse_toml(
            root_table, logging_configs, client_configs, server_configs);
    } catch (const MsgpackRPCException& e) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format(
                "Failed to parse {}: {}", filepath, e.status().message()));
    } catch (const ::toml::parse_error& e) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format("Failed to parse {}: {} (at {}:{})", filepath,
                e.description(), filepath, e.source().begin.line));
    }
}

}  // namespace msgpack_rpc::config::toml
