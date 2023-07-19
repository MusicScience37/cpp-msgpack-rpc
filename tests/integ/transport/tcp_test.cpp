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
 * \brief Test of TCP transport.
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
#include "trompeloeil_catch2.h"

class TestExecutor final : public msgpack_rpc::executors::IExecutor {
public:
    explicit TestExecutor(
        std::shared_ptr<msgpack_rpc::executors::IExecutor> executor)
        : executor_(std::move(executor)) {}

    void run() override { executor_->run(); }

    void run_until_interruption() override {
        executor_->run_until_interruption();
    }

    void stop() override { executor_->stop(); }

    msgpack_rpc::executors::AsioContextType& context(
        msgpack_rpc::executors::OperationType type) noexcept override {
        on_context(type);
        return executor_->context(type);
    }

    MAKE_MOCK1(on_context, void(msgpack_rpc::executors::OperationType));

private:
    std::shared_ptr<msgpack_rpc::executors::IExecutor> executor_;
};

class AcceptorCallbacks
    : public std::enable_shared_from_this<AcceptorCallbacks> {
public:
    MAKE_MOCK1(on_connection,
        void(const std::shared_ptr<msgpack_rpc::transport::IConnection>&));

    void apply_to(
        const std::shared_ptr<msgpack_rpc::transport::IAcceptor>& acceptor) {
        acceptor->start(
            [self = this->shared_from_this()](
                const std::shared_ptr<msgpack_rpc::transport::IConnection>&
                    connection) { self->on_connection(connection); });
    }
};

class ConnectionCallbacks
    : public std::enable_shared_from_this<ConnectionCallbacks> {
public:
    MAKE_MOCK1(on_received, void(msgpack_rpc::messages::ParsedMessage));
    MAKE_MOCK0(on_sent, void());
    MAKE_MOCK1(on_closed, void(msgpack_rpc::Status));

    void apply_to(const std::shared_ptr<msgpack_rpc::transport::IConnection>&
            connection) {
        connection->start(
            [self = this->shared_from_this()](
                msgpack_rpc::messages::ParsedMessage message) {
                self->on_received(std::move(message));
            },
            [self = this->shared_from_this()] { self->on_sent(); },
            [self = this->shared_from_this()](msgpack_rpc::Status status) {
                self->on_closed(std::move(status));
            });
    }
};

TEST_CASE("Communication via TCP") {
    using msgpack_rpc::MsgpackRPCException;
    using msgpack_rpc::Status;
    using msgpack_rpc::StatusCode;
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
    const auto backend = msgpack_rpc::transport::create_tcp_backend(
        executor, MessageParserConfig(), logger);

    const auto acceptor_specified_address = TCPAddress("127.0.0.1", 0);

    ALLOW_CALL(*executor, on_context(OperationType::MAIN));
    const auto post = [&executor](std::function<void()> function) {
        asio::post(executor->context(OperationType::MAIN), std::move(function));
    };

    SECTION("send a message from client to server") {
        const auto method_name = MethodNameView("test_transport");
        const auto message =
            MessageSerializer::serialize_notification(method_name);

        std::shared_ptr<IConnection> client_connection;
        std::shared_ptr<IConnection> server_connection;
        std::shared_ptr<IAcceptor> acceptor;

        const auto client_connection_callbacks =
            std::make_shared<ConnectionCallbacks>();
        const auto server_connection_callbacks =
            std::make_shared<ConnectionCallbacks>();
        const auto acceptor_callbacks = std::make_shared<AcceptorCallbacks>();

        // Start acceptor.
        post([&backend, &acceptor_specified_address, &acceptor,
                 &acceptor_callbacks] {
            acceptor = backend->create_acceptor(acceptor_specified_address);
            acceptor_callbacks->apply_to(acceptor);
        });

        // Connect and send a message.
        post([&backend, &message, &client_connection,
                 &client_connection_callbacks, &acceptor] {
            const auto connector = backend->create_connector();

            connector->async_connect(acceptor->local_address(),
                [&message, &client_connection, &client_connection_callbacks](
                    const Status& status,
                    std::shared_ptr<IConnection> connection) {
                    if (status.code() != msgpack_rpc::StatusCode::SUCCESS) {
                        throw MsgpackRPCException(status);
                    }

                    client_connection = std::move(connection);

                    client_connection_callbacks->apply_to(client_connection);

                    client_connection->async_send(
                        std::make_shared<SerializedMessage>(message));
                });
        });

        REQUIRE_CALL(*acceptor_callbacks, on_connection(_))
            .TIMES(1)
            .WITH(static_cast<bool>(_1))
            .LR_SIDE_EFFECT(server_connection = _1)
            .SIDE_EFFECT(server_connection_callbacks->apply_to(_1));

        std::optional<ParsedMessage> received_message;
        REQUIRE_CALL(*client_connection_callbacks, on_sent()).TIMES(1);
        REQUIRE_CALL(*server_connection_callbacks, on_received(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(received_message = _1)
            .LR_SIDE_EFFECT(server_connection->async_close());

        REQUIRE_CALL(*server_connection_callbacks, on_closed(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(acceptor->stop());
        REQUIRE_CALL(*client_connection_callbacks, on_closed(_)).TIMES(1);

        REQUIRE_CALL(*executor, on_context(OperationType::TRANSPORT))
            .TIMES(AT_LEAST(3));

        executor->run();

        REQUIRE(received_message);
        REQUIRE(std::holds_alternative<ParsedNotification>(*received_message));
        const ParsedNotification notification =
            std::get<ParsedNotification>(*received_message);
        CHECK(notification.method_name().name() == method_name.name());
    }

    SECTION("create acceptor and stop without start") {
        post([&backend, &acceptor_specified_address] {
            const auto acceptor =
                backend->create_acceptor(acceptor_specified_address);

            acceptor->stop();
        });

        REQUIRE_CALL(*executor, on_context(OperationType::TRANSPORT)).TIMES(1);

        executor->run();
    }
}

TEST_CASE("msgpack_rpc::transport::tcp::TCPResolver") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::MessageParserConfig;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto backend = msgpack_rpc::transport::create_tcp_backend(
        executor, MessageParserConfig(), logger);

    const auto resolver = backend->create_resolver();

    SECTION("resolve") {
        const auto uri = URI("tcp", "127.0.0.1", 1234);

        const auto results = resolver->resolve(uri);

        REQUIRE(results.size() == 1);
        CHECK(fmt::format("{}", results.at(0)) == "tcp://127.0.0.1:1234");
    }

    SECTION("try to resolve invalid domain") {
        const auto uri = URI("tcp", "test.invalid", 1234);

        CHECK_THROWS((void)resolver->resolve(uri));
    }
}
