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
 * \brief Test of notifications from clients.
 */
#include <atomic>
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <fmt/format.h>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Notify to methods") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::clients::Client;
    using msgpack_rpc::clients::ClientBuilder;
    using msgpack_rpc::config::ServerConfig;
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    // TODO Parametrize here when additional protocols are tested.
    const auto server_uri = std::string_view("tcp://localhost:0");

    GIVEN("A server") {
        ServerBuilder server_builder{logger};

        server_builder.listen_to(server_uri);

        std::atomic<int> empty_count{0};
        server_builder.add_method<void()>("empty", [&logger, &empty_count] {
            MSGPACK_RPC_INFO(logger, "\"empty\" called.");
            ++empty_count;
        });

        std::atomic<int> one_string_count{0};
        server_builder.add_method<void(std::string)>(
            "one_string", [&logger, &one_string_count](std::string_view str) {
                MSGPACK_RPC_INFO(
                    logger, "\"one_string\" called with \"{}\".", str);
                ++one_string_count;
            });

        std::atomic<int> two_params_count{0};
        server_builder.add_method<void(std::size_t, std::string)>("two_params",
            [&logger, &two_params_count](
                std::size_t number, const std::string& str) {
                MSGPACK_RPC_INFO(logger,
                    "\"two_params\" called with {} and \"{}\".", number, str);
                ++two_params_count;
            });

        auto server = server_builder.build();
        server->start();

        const auto uris = server->local_endpoint_uris();
        MSGPACK_RPC_DEBUG(logger, "Server URIs: {}", fmt::join(uris, ", "));
        REQUIRE(uris != std::vector<URI>{});  // NOLINT

        WHEN("A client is configured correctly") {
            ClientBuilder client_builder{logger};

            for (const auto& uri : uris) {
                client_builder.connect_to(uri);
            }

            Client client = client_builder.build();
            client.start();

            THEN("The client can notify to methods") {
                client.notify("empty");
                client.notify("one_string", "test string 1");
                client.notify("two_params", 3, "test string 2");

                AND_THEN("The server receives notifications") {
                    constexpr auto timeout = std::chrono::seconds(1);
                    constexpr auto check_cycle = std::chrono::milliseconds(10);
                    constexpr auto check_count =
                        static_cast<std::size_t>(timeout / check_cycle);
                    for (std::size_t i = 0; i <= check_count; ++i) {
                        if (empty_count >= 1 && one_string_count >= 1 &&
                            two_params_count >= 1) {
                            break;
                        }
                        std::this_thread::sleep_for(check_cycle);
                    }
                    CHECK(empty_count.load() == 1);
                    CHECK(one_string_count.load() == 1);
                    CHECK(two_params_count.load() == 1);
                }
            }
        }
    }
}
