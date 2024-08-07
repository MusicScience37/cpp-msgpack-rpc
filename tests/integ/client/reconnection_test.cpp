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
 * \brief Test to reconnect clients to servers.
 */
#include <chrono>
#include <ratio>
#include <string_view>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/reconnection_config.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Clients reconnect to servers") {
    using msgpack_rpc::clients::Client;
    using msgpack_rpc::clients::ClientBuilder;
    using msgpack_rpc::config::ClientConfig;
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    // TODO Parametrize here when additional protocols are tested.
    const auto server_uris =
        std::vector<std::string_view>{"tcp://localhost:23400",
            "tcp://localhost:23401", "tcp://localhost:23402"};

    // reconnect soon for faster test
    constexpr auto min_reconnection_waiting_time =
        std::chrono::milliseconds(10);
    constexpr auto max_reconnection_waiting_time =
        std::chrono::milliseconds(20);

    ClientConfig client_config;
    client_config.reconnection().initial_waiting_time(
        min_reconnection_waiting_time);
    client_config.reconnection().max_waiting_time(
        max_reconnection_waiting_time);
    client_config.reconnection().max_jitter_waiting_time(
        min_reconnection_waiting_time);

    GIVEN("No server") {
        WHEN("A client is configured to connect to an endpoint") {
            Client client = ClientBuilder{client_config, logger}
                                .connect_to(server_uris.at(0))
                                .build();

            THEN(
                "The client runs without serious error like death of the "
                "process") {
                constexpr auto test_time = 5 * max_reconnection_waiting_time;
                std::this_thread::sleep_for(test_time);

                WHEN("A server is started after the client starts") {
                    auto server = ServerBuilder{logger}
                                      .listen_to(server_uris.at(0))
                                      .add_method<int(int, int)>("add",
                                          [](int x, int y) { return x + y; })
                                      .build();

                    THEN("The client can call methods") {
                        const int result = client.call<int>("add", 1, 2);

                        CHECK(result == 3);
                    }
                }
            }
        }
    }

    GIVEN("A server and a client connected to the server") {
        auto server = ServerBuilder{logger}
                          .listen_to(server_uris.at(1))
                          .add_method<int(int, int)>(
                              "add", [](int x, int y) { return x + y; })
                          .build();

        Client client = ClientBuilder{client_config, logger}
                            .connect_to(server_uris.at(1))
                            .connect_to(server_uris.at(2))
                            .build();

        // Wait for the connection.
        CHECK(client.call<int>("add", 1, 2) == 3);

        WHEN("The server is shuted down") {
            server.stop();

            THEN(
                "The client runs without serious error like death of the "
                "process") {
                constexpr auto test_time = 5 * max_reconnection_waiting_time;
                std::this_thread::sleep_for(test_time);

                WHEN("A server is restarted") {
                    auto server = ServerBuilder{logger}
                                      .listen_to(server_uris.at(2))
                                      .add_method<int(int, int)>("add",
                                          [](int x, int y) { return x + y; })
                                      .build();

                    THEN("The client can call methods") {
                        const int result = client.call<int>("add", 1, 2);

                        CHECK(result == 3);
                    }
                }
            }
        }
    }
}
