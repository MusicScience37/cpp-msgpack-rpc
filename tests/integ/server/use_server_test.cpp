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
#include <chrono>
#include <cstdio>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <fmt/ranges.h>

#include "../transport/transport_helper.h"
#include "check_connectivity.h"
#include "create_test_logger.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/impl/config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"
#include "msgpack_rpc/transport/backends.h"
#include "msgpack_rpc/transport/i_backend.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "msgpack_rpc/transport/i_connector.h"
#include "trompeloeil_catch2.h"

SCENARIO("Use a server") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::ServerConfig;
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto server_uri = GENERATE(std::string_view("tcp://localhost:0")
#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS
                                         ,
        std::string_view("unix://integ_server_use_server_test.sock")
#endif
    );

    if (server_uri.substr(0, 4) == "unix") {
        (void)std::remove("integ_server_use_server_test.sock");
    }

    GIVEN("A server") {
        ServerBuilder builder{logger};

        builder.listen_to(server_uri);

        builder.add_method<int(int, int)>(
            "add", [](int x, int y) { return x + y; });

        std::promise<std::string> promise;
        auto future = promise.get_future();
        builder.add_method<void(std::string)>(
            "write", [logger, &promise](const std::string& str) {
                MSGPACK_RPC_DEBUG(logger, "Received string: {}", str);
                promise.set_value(str);
            });

        auto server = builder.build();

        const auto uris = server.local_endpoint_uris();
        MSGPACK_RPC_DEBUG(logger, "Server URIs: {}", fmt::join(uris, ", "));
        REQUIRE(uris != std::vector<URI>{});  // NOLINT
        REQUIRE_NOTHROW(check_connectivity(uris));

        WHEN("A client connect to the server") {
            using msgpack_rpc::MsgpackRPCException;
            using msgpack_rpc::Status;
            using msgpack_rpc::config::MessageParserConfig;
            using msgpack_rpc::executors::OperationType;
            using msgpack_rpc::messages::MessageID;
            using msgpack_rpc::messages::MessageSerializer;
            using msgpack_rpc::messages::MethodNameView;
            using msgpack_rpc::messages::ParsedMessage;
            using msgpack_rpc::messages::ParsedResponse;
            using msgpack_rpc::messages::SerializedMessage;
            using msgpack_rpc::transport::IConnection;
            using trompeloeil::_;

            const auto executor =
                msgpack_rpc::executors::create_single_thread_executor(logger);
            const auto post = [&executor](std::function<void()> function) {
                msgpack_rpc::executors::async_invoke(
                    executor, OperationType::CALLBACK, std::move(function));
            };

            std::shared_ptr<msgpack_rpc::transport::IBackend> backend;
            if (uris.front().scheme() == msgpack_rpc::addresses::TCP_SCHEME) {
                backend = msgpack_rpc::transport::create_tcp_backend(
                    executor, MessageParserConfig(), logger);
            }
#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS
            else if (uris.front().scheme() ==
                msgpack_rpc::addresses::UNIX_SOCKET_SCHEME) {
                backend = msgpack_rpc::transport::create_unix_socket_backend(
                    executor, MessageParserConfig(), logger);
            }
#endif
            else {
                FAIL("invalid scheme: " << uris.front().scheme());
            }

            std::shared_ptr<IConnection> client_connection;

            // Connect.
            std::function<void()> on_connected;
            const auto client_connection_callbacks =
                std::make_shared<ConnectionCallbacks>();
            post([&backend, &on_connected, &client_connection,
                     &client_connection_callbacks, uri = uris.front()] {
                const auto connector = backend->create_connector();

                connector->async_connect(uri,
                    [&on_connected, &client_connection,
                        &client_connection_callbacks](const Status& status,
                        std::shared_ptr<IConnection> connection) {
                        if (status.code() != msgpack_rpc::StatusCode::SUCCESS) {
                            throw MsgpackRPCException(status);
                        }

                        client_connection = std::move(connection);

                        client_connection_callbacks->apply_to(
                            client_connection);

                        on_connected();
                    });
            });

            AND_WHEN("The client send a request") {
                const auto request_id = static_cast<MessageID>(12345);
                const auto message = MessageSerializer::serialize_request(
                    "add", request_id, 2, 3);
                on_connected = [&client_connection, &message] {
                    client_connection->async_send(message);
                };

                THEN("Server send a response") {
                    std::optional<ParsedMessage> received_message;
                    REQUIRE_CALL(*client_connection_callbacks, on_sent())
                        .TIMES(1);
                    REQUIRE_CALL(*client_connection_callbacks, on_received(_))
                        .TIMES(1)
                        .LR_SIDE_EFFECT(received_message = _1)
                        .LR_SIDE_EFFECT(client_connection->async_close());
                    REQUIRE_CALL(*client_connection_callbacks, on_closed(_))
                        .TIMES(1);

                    executor->run();

                    REQUIRE(received_message);
                    REQUIRE(std::holds_alternative<ParsedResponse>(
                        *received_message));
                    const auto response =
                        std::get<ParsedResponse>(*received_message);
                    CHECK(response.id() == request_id);
                    CHECK(response.result().result_as<int>() ==
                        5);  // 2 + 3 in request
                }
            }

            AND_WHEN("The client send a notification") {
                const auto message = MessageSerializer::serialize_notification(
                    "write", "Test string.");
                on_connected = [&client_connection, &message] {
                    client_connection->async_send(message);
                };

                THEN("Server receives the message") {
                    REQUIRE_CALL(*client_connection_callbacks, on_sent())
                        .TIMES(1)
                        .LR_SIDE_EFFECT(client_connection->async_close());
                    REQUIRE_CALL(*client_connection_callbacks, on_closed(_))
                        .TIMES(1);

                    executor->run();

                    REQUIRE(future.wait_for(std::chrono::seconds(1)) ==
                        std::future_status::ready);
                    CHECK(future.get() == "Test string.");
                }
            }
        }
    }
}
