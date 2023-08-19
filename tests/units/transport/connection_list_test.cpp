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
 * \brief Test of ConnectionList class.
 */
#include "msgpack_rpc/transport/connection_list.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "mock_connection.h"

TEST_CASE("msgpack_rpc::transport::ConnectionList") {
    using msgpack_rpc::transport::ConnectionList;
    using msgpack_rpc_test::MockConnection;

    ConnectionList<MockConnection> list;

    SECTION("add connections") {
        const auto connection1 = std::make_shared<MockConnection>();
        const auto connection2 = std::make_shared<MockConnection>();

        list.append(connection1);
        list.append(connection2);

        SECTION("close all connections") {
            REQUIRE_CALL(*connection1, async_close()).TIMES(1);
            REQUIRE_CALL(*connection2, async_close()).TIMES(1);

            list.async_close_all();
        }

        SECTION("remove a connection") {
            list.remove(connection1);

            SECTION("close all connections") {
                REQUIRE_CALL(*connection2, async_close()).TIMES(1);

                list.async_close_all();
            }
        }
    }
}
