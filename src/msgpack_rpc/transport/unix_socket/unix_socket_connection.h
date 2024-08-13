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
 * \brief Definition of UnixSocketConnection type.
 */
#pragma once

#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS

#include <asio/local/stream_protocol.hpp>

#include "msgpack_rpc/addresses/unix_socket_address.h"
#include "msgpack_rpc/transport/connection.h"
#include "msgpack_rpc/transport/filepath_acceptor_factory.h"

namespace msgpack_rpc::transport::unix_socket {

//! Type of connections of Unix sockets.
using UnixSocketConnection = Connection<asio::local::stream_protocol::socket,
    addresses::UnixSocketAddress>;

}  // namespace msgpack_rpc::transport::unix_socket

#endif