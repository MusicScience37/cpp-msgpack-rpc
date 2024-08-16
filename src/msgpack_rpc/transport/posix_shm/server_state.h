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
 * \brief Definition of ServerState enumeration.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstdint>

#include <boost/atomic/ipc_atomic.hpp>

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Enumeration of states of servers.
 */
enum class ServerState : std::uint32_t {
    //! The server is initializing.
    INITIALIZING = 0,

    //! The server is running.
    RUNNING = 1,

    //! The server is stopped.
    STOPPED = 2
};

//! Type of atomic variable for ServerState.
using AtomicServerState = boost::atomics::ipc_atomic<ServerState>;

}  // namespace msgpack_rpc::transport::posix_shm

#endif
