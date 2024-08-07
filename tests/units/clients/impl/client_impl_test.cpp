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
 * \brief Test of ClientImpl class.
 */
#include "msgpack_rpc/clients/impl/client_impl.h"

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "../../create_test_logger.h"
#include "../../transport/mock_backend.h"
#include "../../transport/mock_connection.h"
#include "../../transport/mock_connector.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/impl/call_list.h"
#include "msgpack_rpc/clients/impl/client_connector.h"
#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/reconnection_config.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/executors/wrap_executor.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "msgpack_rpc_test/create_parsed_messages.h"
#include "msgpack_rpc_test/parse_messages.h"
#include "trompeloeil_catch2.h"

TEST_CASE("msgpack_rpc::clients::impl::ClientImpl") {
    using msgpack_rpc::Status;
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::clients::impl::CallList;
    using msgpack_rpc::clients::impl::ClientConnector;
    using msgpack_rpc::clients::impl::ClientImpl;
    using msgpack_rpc::clients::impl::ICallFutureImpl;
    using msgpack_rpc::clients::impl::IClientImpl;
    using msgpack_rpc::clients::impl::make_parameters_serializer;
    using msgpack_rpc::config::ReconnectionConfig;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MessageSerializer;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::transport::IConnection;
    using msgpack_rpc_test::create_parsed_successful_response;
    using msgpack_rpc_test::MockBackend;
    using msgpack_rpc_test::MockConnection;
    using msgpack_rpc_test::MockConnector;
    using msgpack_rpc_test::parse_request;
    using trompeloeil::_;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto async_executor = msgpack_rpc::executors::wrap_executor(executor);
    const auto post = [&executor](std::function<void()> function) {
        msgpack_rpc::executors::async_invoke(
            executor, OperationType::CALLBACK, std::move(function));
    };

    constexpr auto request_timeout = std::chrono::seconds(1);
    const auto call_list =
        std::make_shared<CallList>(request_timeout, executor, logger);

    msgpack_rpc::transport::BackendList backends;
    const auto backend = std::make_shared<MockBackend>();
    const auto scheme = std::string_view("protocol1");
    ALLOW_CALL(*backend, scheme()).RETURN(scheme);
    backends.append(backend);

    const auto server_uris = std::vector<URI>{URI(scheme, "host1")};

    SECTION("connect successfully") {
        const auto connection = std::make_shared<MockConnection>();
        IConnection::MessageReceivedCallback on_received =
            [](auto /*message*/) { FAIL(); };
        IConnection::MessageSentCallback on_sent = [] { FAIL(); };
        IConnection::ConnectionClosedCallback on_closed = [](auto /*status*/) {
            FAIL();
        };
        REQUIRE_CALL(*connection, start(_, _, _))
            .TIMES(1)
            .LR_SIDE_EFFECT(on_received = _1)
            .LR_SIDE_EFFECT(on_sent = _2)
            .LR_SIDE_EFFECT(on_closed = _3);
        REQUIRE_CALL(*connection, async_close())
            .TIMES(1)
            .LR_SIDE_EFFECT(on_closed(Status()));

        const auto client_connector = std::make_shared<ClientConnector>(
            executor, backends, server_uris, ReconnectionConfig(), logger);
        const auto client = std::make_shared<ClientImpl>(
            client_connector, call_list, async_executor, logger);

        post([&client] { client->start(); });

        const auto connector = std::make_shared<MockConnector>();
        REQUIRE_CALL(*backend, create_connector()).TIMES(1).RETURN(connector);
        REQUIRE_CALL(*connector, async_connect(_, _))
            .TIMES(1)
            .LR_SIDE_EFFECT(post([on_connect = _2, &connection] {
                on_connect(Status(), connection);
            }));

        SECTION("and no exception is thrown") {
            REQUIRE_NOTHROW(executor->run());
        }

        SECTION("and asynchronously call a method") {
            const auto method_name = MethodNameView("method1");
            const auto param1 = std::string("param1");

            std::shared_ptr<ICallFutureImpl> future;
            post([&client, &method_name, &param1, &future] {
                future = client->async_call(
                    method_name, make_parameters_serializer(param1));
            });

            REQUIRE_CALL(*connection, async_send(_))
                .TIMES(1)
                .LR_SIDE_EFFECT(post(on_sent))
                .LR_SIDE_EFFECT(post([&on_received, serialized_request = _1] {
                    const auto request = parse_request(*serialized_request);
                    const auto request_id = request.id();
                    on_received(create_parsed_successful_response(
                        request_id, "result"));
                }));

            REQUIRE_NOTHROW(executor->run());

            CHECK(future->get_result().result_as<std::string>() == "result");
        }

        SECTION("and notify to a method") {
            const auto method_name = MethodNameView("method2");
            const int param1 = 123;

            post([&client, &method_name, &param1] {
                client->notify(method_name, make_parameters_serializer(param1));
            });

            REQUIRE_CALL(*connection, async_send(_))
                .TIMES(1)
                .LR_SIDE_EFFECT(post(on_sent));

            REQUIRE_NOTHROW(executor->run());
        }
    }

    SECTION("reconnect") {
        const auto connection = std::make_shared<MockConnection>();
        IConnection::MessageReceivedCallback on_received =
            [](auto /*message*/) { FAIL(); };
        IConnection::MessageSentCallback on_sent = [] { FAIL(); };
        IConnection::ConnectionClosedCallback on_closed = [](auto /*status*/) {
            FAIL();
        };
        REQUIRE_CALL(*connection, start(_, _, _))
            .TIMES(1)
            .LR_SIDE_EFFECT(on_received = _1)
            .LR_SIDE_EFFECT(on_sent = _2)
            .LR_SIDE_EFFECT(on_closed = _3);
        REQUIRE_CALL(*connection, async_close())
            .TIMES(1)
            .LR_SIDE_EFFECT(on_closed(Status()));

        const auto client_connector = std::make_shared<ClientConnector>(
            executor, backends, server_uris, ReconnectionConfig(), logger);
        const auto client = std::make_shared<ClientImpl>(
            client_connector, call_list, async_executor, logger);

        post([&client] { client->start(); });

        const auto connector = std::make_shared<MockConnector>();
        REQUIRE_CALL(*backend, create_connector()).TIMES(2).RETURN(connector);
        REQUIRE_CALL(*connector, async_connect(_, _))
            .TIMES(2)
            .LR_SIDE_EFFECT(post([on_connect = _2, &connection] {
                static int calls = 0;
                ++calls;
                if (calls == 1) {
                    on_connect(
                        Status(StatusCode::CONNECTION_FAILURE, "Test error."),
                        nullptr);
                } else {
                    on_connect(Status(), connection);
                }
            }));

        SECTION("and no exception is thrown") {
            REQUIRE_NOTHROW(executor->run());
        }
    }

    SECTION("stop without starting") {
        const auto client_connector = std::make_shared<ClientConnector>(
            executor, backends, server_uris, ReconnectionConfig(), logger);
        const std::shared_ptr<IClientImpl> client =
            std::make_shared<ClientImpl>(
                client_connector, call_list, async_executor, logger);

        REQUIRE_NOTHROW(client->stop());
        REQUIRE_NOTHROW(executor->run());
    }
}
