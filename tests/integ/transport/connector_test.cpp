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
 * \brief Test of connectors.
 */
#include <functional>
#include <memory>
#include <optional>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/transport/backends.h"
#include "msgpack_rpc/transport/i_backend.h"
#include "msgpack_rpc/transport/i_connector.h"

SCENARIO("Create a connector") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::MessageParserConfig;
    using msgpack_rpc::executors::OperationType;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto post = [&executor](std::function<void()> function) {
        msgpack_rpc::executors::async_invoke(
            executor, OperationType::CALLBACK, std::move(function));
    };

    const auto backend = msgpack_rpc::transport::create_tcp_backend(
        executor, MessageParserConfig(), logger);

    GIVEN("A connector") {
        const auto connector = backend->create_connector();

        WHEN(
            "The connector is requested to establish a connection to an "
            "invalid URI") {
            // TLD ".invalid" can't be resolved.
            const auto uri = URI("tcp", "test.invalid", 1234);
            msgpack_rpc::transport::IConnector::ConnectionCallback
                on_connected = [](auto /*status*/, auto /*connection*/) {
                    // This must not be called.
                    FAIL();
                };

            THEN("An exception is thrown") {
                post([&connector, &uri, &on_connected] {
                    REQUIRE_THROWS(connector->async_connect(uri, on_connected));
                });

                executor->run();
            }
        }
    }
}
