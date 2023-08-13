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
 * \brief Test of async_connect function.
 */
#include "msgpack_rpc/transport/async_connect.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "../create_test_logger.h"
#include "mock_backend.h"
#include "mock_connection.h"
#include "mock_connector.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/i_connection.h"

TEST_CASE("msgpack_rpc::transport::MultiBackendConnector") {
    using msgpack_rpc::MsgpackRPCException;
    using msgpack_rpc::Status;
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::transport::async_connect;
    using msgpack_rpc::transport::BackendList;
    using msgpack_rpc::transport::IConnection;
    using msgpack_rpc_test::MockBackend;
    using msgpack_rpc_test::MockConnection;
    using msgpack_rpc_test::MockConnector;
    using trompeloeil::_;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto post = [&executor](std::function<void()> function) {
        msgpack_rpc::executors::async_invoke(
            executor, OperationType::CALLBACK, std::move(function));
    };

    BackendList backends;

    const auto backend1 = std::make_shared<MockBackend>();
    const auto scheme1 = std::string_view("protocol1");
    ALLOW_CALL(*backend1, scheme()).RETURN(scheme1);
    backends.append(backend1);

    const auto backend2 = std::make_shared<MockBackend>();
    const auto scheme2 = std::string_view("protocol2");
    ALLOW_CALL(*backend2, scheme()).RETURN(scheme2);
    backends.append(backend2);

    const auto uris =
        std::vector<URI>{URI(scheme2, "host1"), URI(scheme1, "host2")};

    SECTION("start to connect to URIs") {
        std::function<void(const Status&, std::shared_ptr<IConnection>)>
            on_connection;
        post([&backends, &uris, &on_connection] {
            async_connect(backends, uris, on_connection);
        });

        SECTION("and successfully connect to the first URI") {
            const auto connection = std::make_shared<MockConnection>();
            const auto connector1 = std::make_shared<MockConnector>();
            REQUIRE_CALL(*backend2, create_connector())
                .TIMES(1)
                .RETURN(connector1);
            REQUIRE_CALL(*connector1, async_connect(_, _))
                .TIMES(1)
                .LR_SIDE_EFFECT(post([on_connect = _2, &connection] {
                    on_connect(Status(), connection);
                }));

            std::shared_ptr<IConnection> received_connection;
            on_connection =
                [&received_connection](const Status& status,
                    const std::shared_ptr<IConnection>& connection) {
                    if (status.code() != msgpack_rpc::StatusCode::SUCCESS) {
                        throw MsgpackRPCException(status);
                    }
                    received_connection = connection;
                };

            REQUIRE_NOTHROW(executor->run());

            CHECK(received_connection);
        }

        SECTION("and successfully connect to the second URI") {
            const auto connector1 = std::make_shared<MockConnector>();
            REQUIRE_CALL(*backend2, create_connector())
                .TIMES(1)
                .RETURN(connector1);
            REQUIRE_CALL(*connector1, async_connect(_, _))
                .TIMES(1)
                .LR_SIDE_EFFECT(post([on_connect = _2] {
                    on_connect(
                        Status(StatusCode::CONNECTION_FAILURE, "Test error."),
                        nullptr);
                }));

            const auto connection = std::make_shared<MockConnection>();
            const auto connector2 = std::make_shared<MockConnector>();
            REQUIRE_CALL(*backend1, create_connector())
                .TIMES(1)
                .RETURN(connector2);
            REQUIRE_CALL(*connector2, async_connect(_, _))
                .TIMES(1)
                .LR_SIDE_EFFECT(post([on_connect = _2, &connection] {
                    on_connect(Status(), connection);
                }));

            std::shared_ptr<IConnection> received_connection;
            on_connection =
                [&received_connection](const Status& status,
                    const std::shared_ptr<IConnection>& connection) {
                    if (status.code() != StatusCode::SUCCESS) {
                        throw MsgpackRPCException(status);
                    }
                    received_connection = connection;
                };

            REQUIRE_NOTHROW(executor->run());

            CHECK(received_connection);
        }

        SECTION("and fail to connect to all URIs") {
            const auto connector1 = std::make_shared<MockConnector>();
            REQUIRE_CALL(*backend2, create_connector())
                .TIMES(1)
                .RETURN(connector1);
            REQUIRE_CALL(*connector1, async_connect(_, _))
                .TIMES(1)
                .LR_SIDE_EFFECT(post([on_connect = _2] {
                    on_connect(
                        Status(StatusCode::CONNECTION_FAILURE, "Test error."),
                        nullptr);
                }));

            const auto connector2 = std::make_shared<MockConnector>();
            REQUIRE_CALL(*backend1, create_connector())
                .TIMES(1)
                .RETURN(connector2);
            REQUIRE_CALL(*connector2, async_connect(_, _))
                .TIMES(1)
                .LR_SIDE_EFFECT(post([on_connect = _2] {
                    on_connect(
                        Status(StatusCode::CONNECTION_FAILURE, "Test error."),
                        nullptr);
                }));

            bool on_connection_called = false;
            on_connection =
                [&on_connection_called](const Status& status,
                    const std::shared_ptr<IConnection>& /*connection*/) {
                    CHECK(status.code() == StatusCode::CONNECTION_FAILURE);
                    CHECK_FALSE(on_connection_called);
                    on_connection_called = true;
                };

            REQUIRE_NOTHROW(executor->run());
        }
    }
}
