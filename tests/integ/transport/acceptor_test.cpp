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
 * \brief Test of acceptors.
 */
#include <catch2/catch_test_macros.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/transport/backends.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "transport_helper.h"

SCENARIO("Start and stop acceptor") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::MessageParserConfig;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::transport::IAcceptor;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);

    // TODO Parametrize here when additional protocols are tested.
    const auto backend = msgpack_rpc::transport::create_tcp_backend(
        executor, MessageParserConfig(), logger);
    const URI acceptor_specified_uri = URI::parse("tcp://127.0.0.1:0");

    const auto post = [&executor](std::function<void()> function) {
        asio::post(
            executor->context(OperationType::CALLBACK), std::move(function));
    };

    GIVEN("An acceptor") {
        std::shared_ptr<IAcceptor> acceptor;
        const auto acceptor_callbacks = std::make_shared<AcceptorCallbacks>();

        // Start acceptor.
        post([&backend, &acceptor_specified_uri, &acceptor] {
            const auto acceptors = backend->create_acceptor_factory()->create(
                acceptor_specified_uri);
            REQUIRE(acceptors.size() == 1);
            acceptor = acceptors.front();
        });

        WHEN("The acceptor is started and stopped") {
            post([&acceptor, &acceptor_callbacks] {
                acceptor_callbacks->apply_to(acceptor);
            });
            post([&acceptor] { acceptor->stop(); });

            THEN("No exception isn't thrown") {
                CHECK_NOTHROW(executor->run());
            }
        }

        WHEN("The acceptor is stopped without starting") {
            post([&acceptor] { acceptor->stop(); });

            THEN("No exception isn't thrown") {
                CHECK_NOTHROW(executor->run());
            }
        }

        WHEN("The acceptor is repeatedly stopped") {
            post([&acceptor] { acceptor->stop(); });
            post([&acceptor] { acceptor->stop(); });

            THEN("No exception isn't thrown") {
                CHECK_NOTHROW(executor->run());
            }
        }

        WHEN("The acceptor is started twice") {
            post([&acceptor, &acceptor_callbacks] {
                acceptor_callbacks->apply_to(acceptor);
            });
            post([&acceptor] { acceptor->stop(); });
            post([&acceptor, &acceptor_callbacks] {
                acceptor_callbacks->apply_to(acceptor);
            });

            THEN("An exception is thrown") { CHECK_THROWS(executor->run()); }
        }
    }
}
