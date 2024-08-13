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
 * \brief Definition of common parameters in servers and clients.
 */
#pragma once

#include <string_view>

namespace msgpack_rpc_test {

/*!
 * \brief URI of servers to accept commands for benchmarks.
 *
 * \note THe port number was selected randomly.
 */
constexpr auto COMMAND_SERVER_URI = std::string_view("tcp://localhost:20314");

/*!
 * \brief Type of servers for benchmarks.
 */
enum class ServerType {
    //! TCP of IP version 4.
    TCP4,

    //! TCP of IP version 6.
    TCP6,

    //! Unix socket.
    UNIX_SOCKET,
};

}  // namespace msgpack_rpc_test
