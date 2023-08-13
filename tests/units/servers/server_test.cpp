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
 * \brief Test of Server class..
 */
#include "msgpack_rpc/servers/server.h"

#include <functional>
#include <memory>

#include <asio/post.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../create_test_logger.h"
#include "../methods/mock_method.h"
#include "../transport/mock_acceptor.h"
#include "../transport/mock_connection.h"
#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/method_processor.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "msgpack_rpc_test/create_parsed_messages.h"

TEST_CASE("msgpack_rpc::servers::Server") {
    using msgpack_rpc::addresses::TCPAddress;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MessageSerializer;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc::servers::IServer;
    using msgpack_rpc::servers::Server;
    using msgpack_rpc::transport::IAcceptor;
    using msgpack_rpc::transport::IConnection;
    using msgpack_rpc_test::create_parsed_notification;
    using msgpack_rpc_test::create_parsed_request;
    using msgpack_rpc_test::create_parsed_successful_response;
    using msgpack_rpc_test::MockAcceptor;
    using msgpack_rpc_test::MockConnection;
    using msgpack_rpc_test::MockMethod;
    using trompeloeil::_;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto post_transport = [&executor](std::function<void()> function) {
        asio::post(
            executor->context(OperationType::TRANSPORT), std::move(function));
    };

    const auto executor_wrapper =
        msgpack_rpc::executors::wrap_executor(executor);

    auto method = std::make_unique<MockMethod>();
    auto& method_ref = *method;
    const auto method_name = MethodNameView("test_method");
    ALLOW_CALL(method_ref, name()).RETURN(method_name);

    auto processor = msgpack_rpc::methods::create_method_processor(logger);
    processor->append(std::move(method));

    const auto acceptor = std::make_shared<MockAcceptor>();

    SECTION("start") {
        // Once started, the server will call stop function in the destructor of
        // the server.
        REQUIRE_CALL(*acceptor, stop()).TIMES(1);

        const std::shared_ptr<IServer> server = std::make_shared<Server>(
            std::vector<std::shared_ptr<IAcceptor>>{acceptor},
            std::move(processor), executor_wrapper, logger);

        IAcceptor::ConnectionCallback on_connection{
            [](auto /*connection*/) { FAIL(); }};
        REQUIRE_CALL(*acceptor, start(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(on_connection = _1);

        REQUIRE_NOTHROW(server->start());

        SECTION("then no error happens") { REQUIRE_NOTHROW(executor->run()); }

        SECTION("and accept an connection") {
            const auto remote_address = TCPAddress("127.0.0.1", 20000);
            const auto connection = std::make_shared<MockConnection>();
            ALLOW_CALL(*connection, remote_address()).RETURN(remote_address);

            post_transport(
                [&on_connection, &connection] { on_connection(connection); });

            IConnection::MessageReceivedCallback on_received{
                [](auto /*message*/) { FAIL(); }};
            IConnection::MessageSentCallback on_sent{[]() { FAIL(); }};
            IConnection::ConnectionClosedCallback on_closed{
                [](auto /*status*/) { FAIL(); }};
            std::function<void()> on_connection_handling_started{[] {}};
            REQUIRE_CALL(*connection, start(_, _, _))
                .TIMES(1)
                .LR_SIDE_EFFECT(on_received = _1)
                .LR_SIDE_EFFECT(on_sent = _2)
                .LR_SIDE_EFFECT(on_closed = _3)
                .LR_SIDE_EFFECT(on_connection_handling_started());

            SECTION("then no error happens") {
                REQUIRE_NOTHROW(executor->run());
            }

            SECTION("and receive a request") {
                static constexpr auto request_id =
                    static_cast<MessageID>(12345);
                on_connection_handling_started = [&method_name, &on_received] {
                    const auto request =
                        create_parsed_request(method_name, request_id);
                    on_received(request);
                };

                const auto serialized_response =
                    MessageSerializer::serialize_successful_response(
                        request_id, 0);
                REQUIRE_CALL(method_ref, call(_))
                    .TIMES(1)
                    .RETURN(serialized_response);

                std::function<void()> on_async_send{[] {}};
                REQUIRE_CALL(*connection, async_send(_))
                    .TIMES(1)
                    .LR_SIDE_EFFECT(on_async_send());

                SECTION("then no error happens") {
                    REQUIRE_NOTHROW(executor->run());
                }

                SECTION("and finish to send a response") {
                    on_async_send = [&post_transport, &on_sent] {
                        post_transport([&on_sent] { on_sent(); });
                    };

                    SECTION("then no error happens") {
                        REQUIRE_NOTHROW(executor->run());
                    }
                }
            }

            SECTION("and receive a notification") {
                on_connection_handling_started = [&method_name, &on_received] {
                    const auto notification =
                        create_parsed_notification(method_name);
                    on_received(notification);
                };

                REQUIRE_CALL(method_ref, notify(_)).TIMES(1);

                FORBID_CALL(*connection, async_send(_));

                SECTION("then no error happens") {
                    REQUIRE_NOTHROW(executor->run());
                }
            }

            SECTION("and receive a response") {
                on_connection_handling_started = [&on_received] {
                    const auto response =
                        create_parsed_successful_response(123, 0);
                    on_received(response);
                };

                REQUIRE_CALL(*connection, async_close()).TIMES(1);

                SECTION("then no error happens") {
                    REQUIRE_NOTHROW(executor->run());
                }
            }
        }

        SECTION("and try to start one more time") {
            CHECK_THROWS(server->start());
        }

        SECTION("stop") {
            REQUIRE_NOTHROW(server->stop());

            SECTION("and stop one more time") {
                REQUIRE_NOTHROW(server->stop());
            }
        }
    }

    SECTION("stop without starting") {
        const std::shared_ptr<IServer> server = std::make_shared<Server>(
            std::vector<std::shared_ptr<IAcceptor>>{acceptor},
            std::move(processor), executor_wrapper, logger);

        REQUIRE_NOTHROW(server->stop());
    }
}
