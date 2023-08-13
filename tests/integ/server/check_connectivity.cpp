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
 * \brief Implementation of check_connectivity function.
 */
#include "check_connectivity.h"

#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/system_error.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

/*!
 * \brief Check whether a TCP endpoint is accepting connections.
 *
 * \param[in] uri URI of the TCP endpoint.
 */
void check_connectivity_tcp(const msgpack_rpc::addresses::URI& uri) {
    asio::io_context context;
    asio::ip::tcp::socket socket{context};
    const auto endpoint = asio::ip::tcp::endpoint(
        asio::ip::make_address(uri.host()), uri.port_number().value());

    try {
        socket.connect(endpoint);
    } catch (const asio::system_error& e) {
        throw msgpack_rpc::MsgpackRPCException(
            msgpack_rpc::StatusCode::UNEXPECTED_ERROR,
            fmt::format("Failed to connect to {}: {}.", fmt::streamed(endpoint),
                e.what()));
    }
}

void check_connectivity(const msgpack_rpc::addresses::URI& uri) {
    if (uri.scheme() == msgpack_rpc::addresses::TCP_SCHEME) {
        check_connectivity_tcp(uri);
    }
}

void check_connectivity(const std::vector<msgpack_rpc::addresses::URI>& uris) {
    for (const auto& uri : uris) {
        check_connectivity(uri);
    }
}
