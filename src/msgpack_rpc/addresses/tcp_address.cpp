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
 * \brief Implementation of TCPAddress.
 */
#include "msgpack_rpc/addresses/tcp_address.h"

#include <utility>

#include <asio/ip/address.hpp>

#include "msgpack_rpc/addresses/uri.h"

namespace msgpack_rpc::addresses {

TCPAddress::TCPAddress(std::string_view ip_address, std::uint16_t port_number)
    : TCPAddress(
          AsioTCPAddress(asio::ip::make_address(ip_address), port_number)) {}

TCPAddress::TCPAddress(AsioTCPAddress address) : address_(std::move(address)) {}

std::string TCPAddress::ip_address() const {
    return address_.address().to_string();
}

std::uint16_t TCPAddress::port_number() const { return address_.port(); }

URI TCPAddress::to_uri() const {
    return URI("tcp", address_.address().to_string(), address_.port());
}

const AsioTCPAddress& TCPAddress::asio_address() const { return address_; }

bool TCPAddress::operator==(const TCPAddress& right) const {
    return address_ == right.address_;
}

bool TCPAddress::operator!=(const TCPAddress& right) const {
    return !operator==(right);
}

}  // namespace msgpack_rpc::addresses

namespace fmt {

format_context::iterator
formatter<msgpack_rpc::addresses::TCPAddress>::format(  // NOLINT
    const msgpack_rpc::addresses::TCPAddress& val,
    format_context& context) const {
    return fmt::format_to(context.out(), "{}://{}",
        msgpack_rpc::addresses::TCP_SCHEME, fmt::streamed(val.asio_address()));
}

}  // namespace fmt

std::ostream& operator<<(
    std::ostream& stream, const msgpack_rpc::addresses::TCPAddress& address) {
    stream << address.asio_address();
    return stream;
}
