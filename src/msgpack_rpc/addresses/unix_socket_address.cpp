/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of UnixSocketAddress class.
 */
#include "msgpack_rpc/addresses/unix_socket_address.h"

#if MSGPACK_RPC_HAS_UNIX_SOCKETS

#include <utility>

#include <asio/local/basic_endpoint.hpp>
#include <fmt/format.h>

#include "msgpack_rpc/addresses/schemes.h"

namespace msgpack_rpc::addresses {

UnixSocketAddress::UnixSocketAddress(AsioUnixSocketAddress address)
    : address_(std::move(address)) {}

std::string UnixSocketAddress::filepath() const { return address_.path(); }

URI UnixSocketAddress::to_uri() const {
    return URI(UNIX_SOCKET_SCHEME, address_.path());
}

std::string UnixSocketAddress::to_string() const {
    return fmt::format("unix://{}", address_.path());
}

const AsioUnixSocketAddress& UnixSocketAddress::asio_address() const {
    return address_;
}

bool UnixSocketAddress::operator==(const UnixSocketAddress& right) const {
    return address_ == right.address_;
}

bool UnixSocketAddress::operator!=(const UnixSocketAddress& right) const {
    return !operator==(right);
}

}  // namespace msgpack_rpc::addresses

namespace fmt {

format_context::iterator
formatter<msgpack_rpc::addresses::UnixSocketAddress>::format(  // NOLINT
    const msgpack_rpc::addresses::UnixSocketAddress& val,
    format_context& context) const {
    return fmt::format_to(context.out(), "unix://{}", val.filepath());
}

}  // namespace fmt

std::ostream& operator<<(std::ostream& stream,
    const msgpack_rpc::addresses::UnixSocketAddress& address) {
    stream << "unix://" << address.filepath();
    return stream;
}

#endif
