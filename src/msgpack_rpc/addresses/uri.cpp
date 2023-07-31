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

URI::URI(std::string_view scheme, std::string_view host,
    std::optional<std::uint16_t> port_number)
    : scheme_(scheme), host_(host), port_number_(port_number) {}

std::string_view URI::scheme() const noexcept { return scheme_; }

std::string_view URI::host() const noexcept { return host_; }

std::optional<std::uint16_t> URI::port_number() const noexcept {
    return port_number_;
}

bool URI::operator==(const URI& right) const {
    return (scheme_ == right.scheme_) && (host_ == right.host_) &&
        (port_number_ == right.port_number_);
}

bool URI::operator!=(const URI& right) const { return !operator==(right); }

URI URI::parse(std::string_view uri_string) {
    static re2::RE2 full_regex{R"((tcp)://([a-zA-Z0-9+-.]+):(\d+))"};
    static re2::RE2 ipv6_regex{R"((tcp)://\[([a-zA-Z0-9+-.:]+)\]:(\d+))"};

    std::string scheme{};
    std::string host{};
    auto port = static_cast<std::uint16_t>(0);

    if (!re2::RE2::FullMatch(uri_string, full_regex, &scheme, &host, &port) &&
        !re2::RE2::FullMatch(uri_string, ipv6_regex, &scheme, &host, &port)) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format("Invalid URI string: \"{}\".", uri_string));
    }
    return URI(scheme, host, port);
}

}  // namespace msgpack_rpc::addresses

namespace fmt {

format_context::iterator
formatter<msgpack_rpc::addresses::URI>::format(  // NOLINT
    const msgpack_rpc::addresses::URI& val, format_context& context) const {
    auto out = context.out();
    out = fmt::format_to(out, "{}://", val.scheme());
    if (val.host().find(':') == std::string_view::npos) {
        out = fmt::format_to(out, "{}", val.host());
    } else {
        out = fmt::format_to(out, "[{}]", val.host());
    }
    if (val.port_number()) {
        out = fmt::format_to(out, ":{}", *(val.port_number()));
    }
    return out;
}

}  // namespace fmt

std::ostream& operator<<(
    std::ostream& stream, const msgpack_rpc::addresses::URI& uri) {
    stream << fmt::format("{}", uri);
    return stream;
}
