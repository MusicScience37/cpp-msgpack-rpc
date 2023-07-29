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
 * \brief Implementation of ServerConfig class.
 */
#include "msgpack_rpc/config/server_config.h"

#include <cstdint>
#include <string_view>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/addresses/uri.h"

namespace msgpack_rpc::config {

ServerConfig::ServerConfig() = default;

ServerEndpointConfig& ServerConfig::add_endpoint(const addresses::URI& uri) {
    return endpoints_.emplace_back(uri);
}

ServerEndpointConfig& ServerConfig::add_tcp_endpoint(
    std::string_view host, std::uint16_t port) {
    return add_endpoint(addresses::URI(addresses::TCP_SCHEME, host, port));
}

const std::vector<ServerEndpointConfig>& ServerConfig::endpoints()
    const noexcept {
    return endpoints_;
}

}  // namespace msgpack_rpc::config
