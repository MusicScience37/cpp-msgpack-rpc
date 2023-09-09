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
#include "msgpack_rpc/config/toml/parse_toml.h"

#include <fmt/core.h>
#include <toml++/toml.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/toml/parse_toml_root.h"

namespace msgpack_rpc::config::toml {

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
