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
 * \brief Test to call methods from clients.
 */
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <fmt/ranges.h>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/call_future.h"
#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Call methods") {
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

        server_builder.add_method<int(int, int)>(
            "add", [](int x, int y) { return x + y; });

        server_builder.add_method<std::string(std::string)>(
            "echo", [](const std::string& str) { return "Reply to " + str; });

        server_builder.add_method<std::string()>(
            "get_message", []() { return "Sample text."; });

        int number = 0;
        server_builder.add_method<int()>(
            "get_number", [&number]() { return number; });
        server_builder.add_method<void(int)>(
            "set_number", [&number](int val) { number = val; });

        auto server = server_builder.build();

        const auto uris = server.local_endpoint_uris();
        MSGPACK_RPC_DEBUG(logger, "Server URIs: {}", fmt::join(uris, ", "));
        REQUIRE(uris != std::vector<URI>{});  // NOLINT

        WHEN("A client is configured correctly") {
            ClientBuilder client_builder{logger};

            for (const auto& uri : uris) {
                client_builder.connect_to(uri);
            }

            Client client = client_builder.build();

            THEN(
                "The client can call methods with two parameters "
                "using future") {
                const int result =
                    client.async_call<int>("add", 1, 2).get_result();

                CHECK(result == 3);
            }

            THEN(
                "The client can call methods with two parameters "
                "synchronously") {
                const int result = client.call<int>("add", 1, 2);

                CHECK(result == 3);
            }

            THEN(
                "The client can call methods with one parameter "
                "synchronously") {
                const std::string result =
                    client.call<std::string>("echo", "Hello.");

                CHECK(result == "Reply to Hello.");
            }

            THEN(
                "The client can call methods without parameters "
                "synchronously") {
                const std::string result =
                    client.call<std::string>("get_message");

                CHECK(result == "Sample text.");
            }

            THEN("The client can call methods without results using future") {
                CHECK(client.call<int>("get_number") == 0);

                static constexpr int value = 37;
                CHECK_NOTHROW(
                    client.async_call<void>("set_number", value).get_result());

                CHECK(client.call<int>("get_number") == value);
            }

            THEN("The client can call methods without results synchronously") {
                CHECK(client.call<int>("get_number") == 0);

                static constexpr int value = 37;
                CHECK_NOTHROW(client.call<void>("set_number", value));

                CHECK(client.call<int>("get_number") == value);
            }
        }
    }
}
