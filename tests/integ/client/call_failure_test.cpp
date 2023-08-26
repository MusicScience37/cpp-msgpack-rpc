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
 * \brief Test of failures in method calls.
 */
#include <chrono>
#include <exception>
#include <memory>
#include <ratio>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <msgpack.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/clients/server_exception.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/methods/method_exception.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Call methods to fail") {
    using msgpack_rpc::MsgpackRPCException;
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::clients::Client;
    using msgpack_rpc::clients::ClientBuilder;
    using msgpack_rpc::clients::ServerException;
    using msgpack_rpc::config::ClientConfig;
    using msgpack_rpc::config::ServerConfig;
    using msgpack_rpc::methods::MethodException;
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    // TODO Parametrize here when additional protocols are tested.
    const auto server_uri = std::string_view("tcp://localhost:0");
    GIVEN("A server") {
        ServerBuilder server_builder{logger};

        server_builder.listen_to(server_uri);

        server_builder.add_method<std::string(std::string)>(
            "echo", [](const std::string& str) { return "Reply to " + str; });

        server_builder.add_method<std::string()>(
            "exception", []() -> std::string {
                throw MethodException("Test error in methods.");
            });

        server_builder.add_method<void(double)>("wait", [](double time_sec) {
            std::this_thread::sleep_for(
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::duration<double>(time_sec)));
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

            THEN("The client can call a method with correct parameters") {
                CHECK(client.call<std::string>("echo", "Hello.") ==
                    "Reply to Hello.");
            }

            THEN(
                "The client will receive an exception for wrong parameter "
                "types") {
                CHECK_THROWS_AS(
                    client.call<std::string>("echo", 1), ServerException);
            }

            THEN(
                "The client will receive an exception for wrong result "
                "types") {
                CHECK_THROWS_AS(
                    client.call<int>("echo", "Hello."), MsgpackRPCException);
            }

            THEN(
                "The client will receive an exception for too many "
                "parameters") {
                CHECK_THROWS_AS(client.call<std::string>("echo", "Hello.", 1),
                    ServerException);
            }

            THEN(
                "The client will receive an exception for lack of parameters") {
                CHECK_THROWS_AS(
                    client.call<std::string>("echo"), ServerException);
            }

            THEN("The client will receive an exception occurred in methods") {
                try {
                    (void)client.call<std::string>("exception");
                    FAIL();
                } catch (const ServerException& e) {
                    CHECK(e.error_as<std::string_view>() ==
                        "Test error in methods.");
                }
            }

            THEN(
                "The client will receive an exception occurred in methods "
                "without results") {
                try {
                    (void)client.call<void>("exception");
                    FAIL();
                } catch (const ServerException& e) {
                    CHECK(e.error_as<std::string_view>() ==
                        "Test error in methods.");
                }
            }
        }

        WHEN("A client is configured with small timeout") {
            const auto client_config =
                ClientConfig().call_timeout(std::chrono::milliseconds(1));
            ClientBuilder client_builder{client_config, logger};

            for (const auto& uri : uris) {
                client_builder.connect_to(uri);
            }

            Client client = client_builder.build();
            client.start();

            THEN("The client will receive an exception for timeout") {
                const double param = 0.01;
                try {
                    (void)client.call<std::string>("wait", param);
                    FAIL();
                } catch (const MsgpackRPCException& e) {
                    CHECK(e.status().code() == StatusCode::TIMEOUT);
                }
            }
        }
    }
}
