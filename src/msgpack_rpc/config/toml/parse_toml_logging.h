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
 * \brief Definition of parse_toml functions for logging.
 */
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <toml++/toml.h>

#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/toml/parse_toml_common.h"
#include "msgpack_rpc/logging/log_level.h"

namespace msgpack_rpc::config::toml::impl {

/*!
 * \brief Parse a log level from a string.
 *
 * \param[in] str String.
 * \param[in] source Location in TOML. (For errors.)
 * \param[in] config_key Key of the configuration.
 * \return Log level.
 */
[[nodiscard]] inline logging::LogLevel parse_log_level(std::string_view str,
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
inline void parse_toml(const ::toml::table& table, LoggingConfig& config) {
    for (const auto& [key, value] : table) {
        const auto key_str = key.str();
        if (key_str == "file_path") {
            MSGPACK_RPC_PARSE_TOML_VALUE("file_path", file_path, std::string);
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
inline void parse_toml(const ::toml::table& table,
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

}  // namespace msgpack_rpc::config::toml::impl
