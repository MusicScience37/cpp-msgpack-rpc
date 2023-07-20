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
 * \brief Test of sending messages..
 */
#include <functional>
#include <memory>
#include <optional>
#include <variant>

#include <asio/error_code.hpp>
#include <asio/post.hpp>
#include <asio/system_error.hpp>
#include <catch2/catch_test_macros.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/transport/backends.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "transport_helper.h"
#include "trompeloeil_catch2.h"

SCENARIO("Send messages") {
    using msgpack_rpc::MsgpackRPCException;
    using msgpack_rpc::Status;
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::addresses::Address;
    using msgpack_rpc::addresses::TCPAddress;
    using msgpack_rpc::config::MessageParserConfig;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::messages::MessageSerializer;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::messages::ParsedMessage;
    using msgpack_rpc::messages::ParsedNotification;
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc::transport::IAcceptor;
    using msgpack_rpc::transport::IConnection;
    using trompeloeil::_;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor = std::make_shared<TestExecutor>(
        msgpack_rpc::executors::create_single_thread_executor(logger));

    // TODO Parametrize here when additional protocols are tested.
    const auto backend = msgpack_rpc::transport::create_tcp_backend(
        executor, MessageParserConfig(), logger);
    const Address acceptor_specified_address = TCPAddress("127.0.0.1", 0);

    ALLOW_CALL(*executor, on_context(OperationType::MAIN));
    const auto post = [&executor](std::function<void()> function) {
        asio::post(executor->context(OperationType::MAIN), std::move(function));
    };

    GIVEN("A connection is established") {
        std::shared_ptr<IConnection> client_connection;
        std::shared_ptr<IConnection> server_connection;
        std::shared_ptr<IAcceptor> acceptor;

        const auto client_connection_callbacks =
            std::make_shared<ConnectionCallbacks>();
        const auto server_connection_callbacks =
            std::make_shared<ConnectionCallbacks>();
        const auto acceptor_callbacks = std::make_shared<AcceptorCallbacks>();

        std::function<void()> on_connected;

        // Start acceptor.
        post([&backend, &acceptor_specified_address, &acceptor,
                 &acceptor_callbacks] {
            acceptor = backend->create_acceptor(acceptor_specified_address);
            acceptor_callbacks->apply_to(acceptor);
        });

        // Connect.
        post([&backend, &on_connected, &client_connection,
                 &client_connection_callbacks, &acceptor] {
            const auto connector = backend->create_connector();

            connector->async_connect(acceptor->local_address(),
                [&on_connected, &client_connection,
                    &client_connection_callbacks](const Status& status,
                    std::shared_ptr<IConnection> connection) {
                    if (status.code() != msgpack_rpc::StatusCode::SUCCESS) {
                        throw MsgpackRPCException(status);
                    }

                    client_connection = std::move(connection);

                    client_connection_callbacks->apply_to(client_connection);

                    on_connected();
                });
        });

        std::optional<Address> acceptor_local_address;
        std::optional<Address> server_local_address;
        std::optional<Address> server_remote_address;
        REQUIRE_CALL(*acceptor_callbacks, on_connection(_))
            .TIMES(1)
            .WITH(static_cast<bool>(_1))
            .LR_SIDE_EFFECT(server_connection = _1)
            .LR_SIDE_EFFECT(acceptor_local_address = acceptor->local_address())
            .LR_SIDE_EFFECT(
                server_local_address = server_connection->local_address())
            .LR_SIDE_EFFECT(
                server_remote_address = server_connection->remote_address())
            .SIDE_EFFECT(server_connection_callbacks->apply_to(_1));

        REQUIRE_CALL(*executor, on_context(OperationType::TRANSPORT)).TIMES(4);

        REQUIRE_CALL(*server_connection_callbacks, on_closed(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(acceptor->stop());
        REQUIRE_CALL(*client_connection_callbacks, on_closed(_)).TIMES(1);

        THEN("addresses are correctly set") {
            std::optional<Address> client_local_address;
            std::optional<Address> client_remote_address;

            on_connected = [&client_connection, &client_local_address,
                               &client_remote_address] {
                client_local_address = client_connection->local_address();
                client_remote_address = client_connection->remote_address();

                client_connection->async_close();
            };

            executor->run();

            CHECK(
                server_local_address.value() == acceptor_local_address.value());
            CHECK(
                server_local_address.value() == client_remote_address.value());
            CHECK(
                server_remote_address.value() != server_local_address.value());
            CHECK(
                server_remote_address.value() == client_local_address.value());
        }

        WHEN("A message is sent from client-side connection") {
            const auto method_name = MethodNameView("test_transport");
            const auto message =
                MessageSerializer::serialize_notification(method_name);

            on_connected = [&client_connection, &message] {
                client_connection->async_send(
                    std::make_shared<SerializedMessage>(message));
            };

            THEN("Server-side connection received the message") {
                std::optional<ParsedMessage> received_message;
                REQUIRE_CALL(*client_connection_callbacks, on_sent()).TIMES(1);
                REQUIRE_CALL(*server_connection_callbacks, on_received(_))
                    .TIMES(1)
                    .LR_SIDE_EFFECT(received_message = _1)
                    .LR_SIDE_EFFECT(server_connection->async_close());

                executor->run();

                REQUIRE(received_message);
                REQUIRE(std::holds_alternative<ParsedNotification>(
                    *received_message));
                const ParsedNotification notification =
                    std::get<ParsedNotification>(*received_message);
                CHECK(notification.method_name().name() == method_name.name());
            }
        }
    }
}
