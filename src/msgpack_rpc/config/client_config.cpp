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
 * \brief Implementation of ClientConfig class.
 */
#include "msgpack_rpc/config/client_config.h"

#include <chrono>
#include <string_view>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::config {

namespace {

constexpr auto CLIENT_CONFIG_CALL_TIMEOUT = std::chrono::seconds(15);

}  // namespace

ClientConfig::ClientConfig() : call_timeout_(CLIENT_CONFIG_CALL_TIMEOUT) {}

ClientConfig& ClientConfig::add_uri(const addresses::URI& uri) {
    uris_.push_back(uri);
    return *this;
}

ClientConfig& ClientConfig::add_uri(std::string_view uri) {
    return add_uri(addresses::URI::parse(uri));
}

const std::vector<addresses::URI>& ClientConfig::uris() const noexcept {
    return uris_;
}

ClientConfig& ClientConfig::call_timeout(std::chrono::nanoseconds value) {
    if (value <= std::chrono::nanoseconds(0)) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Duration of timeout must be longer than zero.");
    }
    call_timeout_ = value;
    return *this;
}

std::chrono::nanoseconds ClientConfig::call_timeout() const noexcept {
    return call_timeout_;
}

MessageParserConfig& ClientConfig::message_parser() noexcept {
    return message_parser_;
}

const MessageParserConfig& ClientConfig::message_parser() const noexcept {
    return message_parser_;
}

ExecutorConfig& ClientConfig::executor() noexcept { return executor_; }

const ExecutorConfig& ClientConfig::executor() const noexcept {
    return executor_;
}

ReconnectionConfig& ClientConfig::reconnection() noexcept {
    return reconnection_;
}

const ReconnectionConfig& ClientConfig::reconnection() const noexcept {
    return reconnection_;
}

}  // namespace msgpack_rpc::config
