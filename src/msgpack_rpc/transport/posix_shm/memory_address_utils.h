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
 * \brief Definitions of utility functions of memory addresses.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>

namespace msgpack_rpc::transport::posix_shm {

//! Alignment of cache line.
constexpr std::size_t CACHE_LINE_ALIGNMENT = 64;

/*!
 * \brief Calculate the address of the next object.
 *
 * \param[in] last_address Address of the end of the last object.
 * \param[in] next_alignment Alignment of the next object.
 * \return Address of the next object.
 */
inline constexpr std::size_t calc_next_object_address(
    std::size_t last_address, std::size_t next_alignment) {
    return ((last_address + next_alignment - 1) / next_alignment) *
        next_alignment;
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif