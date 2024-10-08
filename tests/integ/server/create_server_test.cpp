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
#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <fmt/ranges.h>

#include "check_connectivity.h"
#include "create_test_logger.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Create a server") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::ServerConfig;
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto server_uri = GENERATE(std::string_view("tcp://localhost:0")
#if MSGPACK_RPC_HAS_UNIX_SOCKETS
                                         ,
        std::string_view("unix://integ_server_create_server_test.sock")
#endif
    );

    if (server_uri.substr(0, 4) == "unix") {
        (void)std::remove("integ_server_create_server_test.sock");
    }

    GIVEN("A default configuration") {
        const ServerConfig config;

        THEN("Server builder can be created") {
            ServerBuilder builder{config, logger};

            WHEN("A URI is added") {
                builder.listen_to(server_uri);

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

                        AND_THEN("The server can be connectable") {
                            const auto uris = server.local_endpoint_uris();
                            MSGPACK_RPC_DEBUG(logger, "Server URIs: {}",
                                fmt::join(uris, ", "));
                            CHECK(uris != std::vector<URI>{});  // NOLINT

                            CHECK_NOTHROW(check_connectivity(uris));
                        }
                    }
                }
            }

            WHEN("No URI is added") {
                THEN("Server cannot be built") {
                    CHECK_THROWS((void)builder.build());
                }
            }
        }
    }

    GIVEN("A configuration with a URI") {
        ServerConfig config;
        config.add_uri(server_uri);

        THEN("Server builder can be created") {
            ServerBuilder builder{config, logger};

            AND_WHEN("Some methods are added") {
                builder.add_method<int(int, int)>(
                    "add", [](int x, int y) { return x + y; });
                builder.add_method<void(std::string)>(
                    "write", [logger](const std::string& str) {
                        MSGPACK_RPC_DEBUG(logger, "Received string: {}", str);
                    });

                THEN("Server can be built") {
                    auto server = builder.build();

                    AND_THEN("The server can be connectable") {
                        const auto uris = server.local_endpoint_uris();
                        MSGPACK_RPC_DEBUG(
                            logger, "Server URIs: {}", fmt::join(uris, ", "));
                        CHECK(uris != std::vector<URI>{});  // NOLINT

                        CHECK_NOTHROW(check_connectivity(uris));
                    }
                }
            }
        }
    }
}
