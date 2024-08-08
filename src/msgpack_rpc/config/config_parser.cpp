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
 * \brief Implementation of ConfigParser class.
 */
#include "msgpack_rpc/config/config_parser.h"

#include <fmt/format.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/toml/parse_toml.h"

namespace msgpack_rpc::config {

ConfigParser::ConfigParser() = default;

void ConfigParser::parse(std::string_view filepath) {
    toml::parse_toml(
        filepath, logging_configs_, client_configs_, server_configs_);
}

const LoggingConfig& ConfigParser::logging_config(std::string_view name) const {
    try {
        return logging_configs_.at(std::string(name));
    } catch (...) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format("Configuration of logging not found: {}.", name));
    }
}

const ClientConfig& ConfigParser::client_config(std::string_view name) const {
    try {
        return client_configs_.at(std::string(name));
    } catch (...) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format("Configuration of client not found: {}.", name));
    }
}

const ServerConfig& ConfigParser::server_config(std::string_view name) const {
    try {
        return server_configs_.at(std::string(name));
    } catch (...) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format("Configuration of server not found: {}.", name));
    }
}

}  // namespace msgpack_rpc::config
