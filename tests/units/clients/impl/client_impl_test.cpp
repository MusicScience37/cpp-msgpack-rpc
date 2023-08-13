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

#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <trompeloeil.hpp>

#include "../../create_test_logger.h"
#include "../../transport/mock_backend.h"
#include "../../transport/mock_connection.h"
#include "../../transport/mock_connector.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/impl/client_connector.h"
#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/i_connection.h"

TEST_CASE("msgpack_rpc::clients::impl::ClientImpl") {
    using msgpack_rpc::Status;
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::clients::impl::ClientConnector;
    using msgpack_rpc::clients::impl::ClientImpl;
    using msgpack_rpc::clients::impl::ICallFutureImpl;
    using msgpack_rpc::clients::impl::IClientImpl;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MessageSerializer;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::transport::IConnection;
    using msgpack_rpc_test::MockBackend;
    using msgpack_rpc_test::MockConnection;
    using msgpack_rpc_test::MockConnector;
    using trompeloeil::_;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto async_executor = msgpack_rpc::executors::wrap_executor(executor);
    const auto post = [&executor](std::function<void()> function) {
        msgpack_rpc::executors::async_invoke(
            executor, OperationType::CALLBACK, std::move(function));
    };

    msgpack_rpc::transport::BackendList backends;
    const auto backend = std::make_shared<MockBackend>();
    const auto scheme = std::string_view("protocol1");
    ALLOW_CALL(*backend, scheme()).RETURN(scheme);
    backends.append(backend);

    const auto server_uris = std::vector<URI>{URI(scheme, "host1")};

    const auto client_connector = std::make_shared<ClientConnector>(
        executor, backends, server_uris, logger);
    const std::shared_ptr<IClientImpl> client =
        std::make_shared<ClientImpl>(client_connector, async_executor, logger);

    SECTION("connect successfully") {
        post([&client] { client->start(); });

        const auto connection = std::make_shared<MockConnection>();
        IConnection::MessageReceivedCallback on_received =
            [](auto /*message*/) { FAIL(); };
        IConnection::MessageSentCallback on_sent = [] { FAIL(); };
        IConnection::ConnectionClosedCallback on_closed = [](auto /*message*/) {
            FAIL();
        };
        REQUIRE_CALL(*connection, start(_, _, _))
            .TIMES(1)
            .LR_SIDE_EFFECT(on_received = _1)
            .LR_SIDE_EFFECT(on_sent = _2)
            .LR_SIDE_EFFECT(on_closed = _3);

        const auto connector = std::make_shared<MockConnector>();
        REQUIRE_CALL(*backend, create_connector()).TIMES(1).RETURN(connector);
        REQUIRE_CALL(*connector, async_connect(_, _))
            .TIMES(1)
            .LR_SIDE_EFFECT(post([on_connect = _2, &connection] {
                on_connect(Status(), connection);
            }));

        SECTION("and no exception thrown") { REQUIRE_NOTHROW(executor->run()); }

        SECTION("and asynchronously call a method") {
            const auto method_name = MethodNameView("method1");
            const auto request_id = static_cast<MessageID>(12345);
            const auto param1 = std::string("param1");
            const auto serialized_request =
                MessageSerializer::serialize_request(
                    method_name, request_id, param1);

            post([&client, &method_name, &serialized_request] {
                const std::shared_ptr<ICallFutureImpl> future =
                    client->async_call(
                        method_name, request_id, serialized_request);
            });

            REQUIRE_NOTHROW(executor->run());
        }
    }
}
