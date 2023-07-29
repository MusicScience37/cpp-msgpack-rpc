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

ServerConfig& ServerConfig::add_uri(const addresses::URI& uri) {
    uris_.push_back(uri);
    return *this;
}

ServerConfig& ServerConfig::add_uri(std::string_view uri) {
    return add_uri(addresses::URI::parse(uri));
}

const std::vector<addresses::URI>& ServerConfig::uris() const noexcept {
    return uris_;
}

MessageParserConfig& ServerConfig::message_parser() noexcept {
    return message_parser_;
}

const MessageParserConfig& ServerConfig::message_parser() const noexcept {
    return message_parser_;
}

ExecutorConfig& ServerConfig::executor() noexcept { return executor_; }

const ExecutorConfig& ServerConfig::executor() const noexcept {
    return executor_;
}

}  // namespace msgpack_rpc::config
