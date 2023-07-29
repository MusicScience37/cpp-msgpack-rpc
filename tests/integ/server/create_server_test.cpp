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
 * \brief Test of servers.
 */
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Create a server") {
    using msgpack_rpc::config::ServerConfig;
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    GIVEN("A default configuration") {
        ServerConfig config;

        THEN("Server builder can be created") {
            ServerBuilder builder{config, logger};

            WHEN("A URI is added") {
                builder.listen_to("tcp://127.0.0.1:0");

                AND_WHEN("Some methods are added") {
                    builder.add_method<int(int, int)>(
                        "add", [](int x, int y) { return x + y; });
                    builder.add_method<void(std::string)>(
                        "write", [logger](const std::string& str) {
                            MSGPACK_RPC_DEBUG(
                                logger, "Received string: {}", str);
                        });

                    THEN("Server can be built") {
                        auto server = builder.build();
                    }
                }
            }
        }
    }
}
