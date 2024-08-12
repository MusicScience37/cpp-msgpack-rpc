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
 * \brief Definition of TCPAcceptor type.
 */
#pragma once

#include <asio/ip/basic_endpoint.hpp>
#include <asio/ip/tcp.hpp>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/transport/acceptor.h"

namespace msgpack_rpc::transport::tcp {

//! Type of acceptors of TCP.
using TCPAcceptor = Acceptor<asio::ip::tcp::acceptor, asio::ip::tcp::socket,
    addresses::TCPAddress>;

}  // namespace msgpack_rpc::transport::tcp
