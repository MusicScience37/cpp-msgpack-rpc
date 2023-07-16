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
 * \brief Implementation of URI class.
 */
#include "msgpack_rpc/addresses/uri.h"

#include <fmt/format.h>
#include <re2/re2.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::addresses {

URI::URI(
    std::string_view schema, std::string_view host, std::uint16_t port_number)
    : schema_(schema), host_(host), port_number_(port_number) {}

std::string_view URI::schema() const noexcept { return schema_; }

std::string_view URI::host() const noexcept { return host_; }

std::uint16_t URI::port_number() const noexcept { return port_number_; }

URI URI::parse(std::string_view uri_string) {
    static re2::RE2 full_regex{
        R"(([a-zA-Z0-9+-.]+)://([a-zA-Z0-9+-.]+):(\d+))"};
    static re2::RE2 ipv6_regex{
        R"(([a-zA-Z0-9+-.]+)://\[([a-zA-Z0-9+-.:]+)\]:(\d+))"};
    static re2::RE2 no_port_regex{R"(([a-zA-Z0-9+-.]+)://([a-zA-Z0-9+-.]+))"};

    std::string schema{};
    std::string host{};
    auto port = static_cast<std::uint16_t>(0);

    if (!re2::RE2::FullMatch(uri_string, full_regex, &schema, &host, &port) &&
        !re2::RE2::FullMatch(uri_string, ipv6_regex, &schema, &host, &port) &&
        !re2::RE2::FullMatch(uri_string, no_port_regex, &schema, &host)) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format("Invalid URI string: \"{}\".", uri_string));
    }
    return URI(schema, host, port);
}

}  // namespace msgpack_rpc::addresses
