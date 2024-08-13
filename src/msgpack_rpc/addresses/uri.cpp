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

#include <ostream>

#include <absl/strings/string_view.h>
#include <fmt/format.h>
#include <re2/re2.h>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::addresses {

URI::URI(std::string_view scheme, std::string_view host_or_file_path,
    std::optional<std::uint16_t> port_number)
    : scheme_(scheme),
      host_or_file_path_(host_or_file_path),
      port_number_(port_number) {}

std::string_view URI::scheme() const noexcept { return scheme_; }

std::string_view URI::host_or_file_path() const noexcept {
    return host_or_file_path_;
}

std::optional<std::uint16_t> URI::port_number() const noexcept {
    return port_number_;
}

bool URI::operator==(const URI& right) const {
    return (scheme_ == right.scheme_) &&
        (host_or_file_path_ == right.host_or_file_path_) &&
        (port_number_ == right.port_number_);
}

bool URI::operator!=(const URI& right) const { return !operator==(right); }

URI URI::parse(std::string_view uri_string) {
    static re2::RE2 ip_regex{R"((tcp)://([a-zA-Z0-9+-.]+):(\d+))"};
    static re2::RE2 ipv6_regex{R"((tcp)://\[([a-zA-Z0-9+-.:]+)\]:(\d+))"};
    static re2::RE2 file_path_regex{R"((unix)://(.+))"};
    static re2::RE2 file_name_regex{R"((shm)://([^/]+))"};

    std::string scheme{};
    std::string host{};
    auto port = static_cast<std::uint16_t>(0);

    const auto absl_uri_string =
        absl::string_view(uri_string.data(), uri_string.size());
    if (!re2::RE2::FullMatch(
            absl_uri_string, ip_regex, &scheme, &host, &port) &&
        !re2::RE2::FullMatch(
            absl_uri_string, ipv6_regex, &scheme, &host, &port) &&
        !re2::RE2::FullMatch(
            absl_uri_string, file_path_regex, &scheme, &host) &&
        !re2::RE2::FullMatch(
            absl_uri_string, file_name_regex, &scheme, &host)) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format("Invalid URI string: \"{}\".", uri_string));
    }
    if (scheme == TCP_SCHEME) {
        return URI(scheme, host, port);
    }
    if (scheme == UNIX_SOCKET_SCHEME || scheme == SHARED_MEMORY_SCHEME) {
        return URI(scheme, host);
    }
    throw MsgpackRPCException(
        StatusCode::UNEXPECTED_ERROR, "Unexpected line is executed.");
}

}  // namespace msgpack_rpc::addresses

namespace fmt {

format_context::iterator
formatter<msgpack_rpc::addresses::URI>::format(  // NOLINT
    const msgpack_rpc::addresses::URI& val, format_context& context) const {
    auto out = context.out();
    out = fmt::format_to(out, "{}://", val.scheme());
    if (val.scheme() == msgpack_rpc::addresses::TCP_SCHEME) {
        if (val.host_or_file_path().find(':') == std::string_view::npos) {
            out = fmt::format_to(out, "{}", val.host_or_file_path());
        } else {
            out = fmt::format_to(out, "[{}]", val.host_or_file_path());
        }
    } else {
        out = fmt::format_to(out, "{}", val.host_or_file_path());
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
