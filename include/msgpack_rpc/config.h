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
 * \brief Definitions of platform-specific macros.
 */
#pragma once

#include "msgpack_rpc/impl/config.h"  // IWYU pragma: export

/*!
 * \brief Macro to check whether this build of cpp-msgpack-rpc library has the
 * feature of RPC using Unix sockets.
 *
 * 1 means enabled, 0 means disabled.
 */
#define MSGPACK_RPC_HAS_UNIX_SOCKETS MSGPACK_RPC_ENABLE_UNIX_SOCKETS

/*!
 * \brief Macro to check whether this build of cpp-msgpack-rpc library has the
 * feature of RPC on POSIX shared memory.
 *
 * 1 means enabled, 0 means disabled.
 */
#define MSGPACK_RPC_HAS_POSIX_SHM MSGPACK_RPC_ENABLE_POSIX_SHM

/*!
 * \brief Macro to check whether this build of cpp-msgpack-rpc library has the
 * feature of RPC on shared memory.
 *
 * 1 means enabled, 0 means disabled.
 */
#define MSGPACK_RPC_HAS_SHM MSGPACK_RPC_ENABLE_POSIX_SHM
