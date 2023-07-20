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
#include "transport_helper.h"
#include "trompeloeil_catch2.h"

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
