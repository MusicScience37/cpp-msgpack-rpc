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
 * \brief Test of ClientMemoryAddressCalculator class.
 */
#include "msgpack_rpc/transport/posix_shm/memory_address_utils.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::transport::posix_shm::calc_next_object_address") {
    using msgpack_rpc::transport::posix_shm::calc_next_object_address;

    SECTION("calculate") {
        CHECK(calc_next_object_address(14U, 16U) == 16U);
        CHECK(calc_next_object_address(15U, 16U) == 16U);
        CHECK(calc_next_object_address(16U, 16U) == 16U);
        CHECK(calc_next_object_address(17U, 16U) == 32U);
        CHECK(calc_next_object_address(18U, 16U) == 32U);
        CHECK(calc_next_object_address(30U, 16U) == 32U);
        CHECK(calc_next_object_address(31U, 16U) == 32U);
        CHECK(calc_next_object_address(32U, 16U) == 32U);
        CHECK(calc_next_object_address(33U, 16U) == 48U);
        CHECK(calc_next_object_address(34U, 16U) == 48U);
    }
}

#endif
