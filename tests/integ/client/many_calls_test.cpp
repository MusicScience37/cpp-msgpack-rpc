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
 * \brief Test to call methods many times from clients.
 */
#include <chrono>
#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/call_future.h"
#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Call methods many times") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::clients::CallFuture;
    using msgpack_rpc::clients::Client;
    using msgpack_rpc::clients::ClientBuilder;
    using msgpack_rpc::config::ClientConfig;
    using msgpack_rpc::config::ServerConfig;
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    // TODO Parametrize here when additional protocols are tested.
    const auto server_uri = std::string_view("tcp://localhost:0");

    GIVEN("A server") {
        ServerBuilder server_builder{logger};

        server_builder.listen_to(server_uri);

        server_builder.add_method<std::size_t(std::size_t)>(
            "echo", [&logger](std::size_t x) {
                MSGPACK_RPC_INFO(logger, "Received {}.", x);
                return x;
            });

        auto server = server_builder.build();
        server.start();

        const auto uris = server.local_endpoint_uris();
        MSGPACK_RPC_DEBUG(logger, "Server URIs: {}", fmt::join(uris, ", "));
        REQUIRE(uris != std::vector<URI>{});  // NOLINT

        WHEN("A client is configured correctly") {
            ClientBuilder client_builder{
                ClientConfig().call_timeout(std::chrono::seconds(1)), logger};

            for (const auto& uri : uris) {
                client_builder.connect_to(uri);
            }

            Client client = client_builder.build();
            client.start();

            AND_WHEN("The client calls a method multiple times") {
                constexpr std::size_t count = 100;
                std::vector<CallFuture<std::size_t>> futures;
                futures.reserve(count);
                for (std::size_t i = 0; i < count; ++i) {
                    INFO("i = " << i);
                    REQUIRE_NOTHROW(futures.push_back(
                        client.async_call<std::size_t>("echo", i)));
                }

                THEN("All calls are processed properly") {
                    for (std::size_t i = 0; i < count; ++i) {
                        INFO("i = " << i);
                        CHECK(futures.at(i).get_result() == i);
                    }
                }
            }
        }
    }
}
