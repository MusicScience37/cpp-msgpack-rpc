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
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <fmt/format.h>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/impl/config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/backends.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_acceptor_factory.h"
#include "msgpack_rpc/transport/i_backend.h"
#include "transport_helper.h"

SCENARIO("Start and stop acceptor") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::MessageParserConfig;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::transport::IAcceptor;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);

    const URI acceptor_specified_uri = GENERATE(URI::parse("tcp://127.0.0.1:0")
#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS
                                                    ,
        URI::parse("unix://integ_transport_acceptor_test.sock")
#endif
    );
    INFO("URI: " << fmt::to_string(acceptor_specified_uri));
    MSGPACK_RPC_INFO(logger, "URI: {}", acceptor_specified_uri);

    std::shared_ptr<msgpack_rpc::transport::IBackend> backend;
    if (acceptor_specified_uri.scheme() == msgpack_rpc::addresses::TCP_SCHEME) {
        backend = msgpack_rpc::transport::create_tcp_backend(
            executor, MessageParserConfig(), logger);
    }
#if MSGPACK_RPC_ENABLE_UNIX_SOCKETS
    else if (acceptor_specified_uri.scheme() ==
        msgpack_rpc::addresses::UNIX_SOCKET_SCHEME) {
        (void)std::remove(
            static_cast<std::string>(acceptor_specified_uri.host_or_filepath())
                .c_str());
        backend = msgpack_rpc::transport::create_unix_socket_backend(
            executor, MessageParserConfig(), logger);
    }
#endif
    else {
        FAIL("invalid scheme: " << acceptor_specified_uri.scheme());
    }

    const auto post = [&executor](std::function<void()> function) {
        msgpack_rpc::executors::async_invoke(
            executor, OperationType::CALLBACK, std::move(function));
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
