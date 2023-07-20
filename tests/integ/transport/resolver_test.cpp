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
 * \brief Test of resolvers.
 */
#include <catch2/catch_test_macros.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/transport/backends.h"

SCENARIO("Resolve addresses in TCP") {
    using msgpack_rpc::addresses::TCPAddress;
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::config::MessageParserConfig;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);

    const auto backend = msgpack_rpc::transport::create_tcp_backend(
        executor, MessageParserConfig(), logger);

    GIVEN("A resolver") {
        const auto resolver = backend->create_resolver();

        WHEN(
            "The resolver is requested to resolve a URI with a valid IP "
            "address") {
            const auto uri = URI("tcp", "127.0.0.1", 1234);

            THEN("Correctly generate an address") {
                const auto results = resolver->resolve(uri);

                REQUIRE(results.size() == 1);
                CHECK(results.at(0) == TCPAddress("127.0.0.1", 1234));
            }
        }

        WHEN("The resolver is requested to resolve an invalid URI") {
            // TLD ".invalid" can't be resolved.
            const auto uri = URI("tcp", "test.invalid", 1234);

            THEN("An exception is thrown") {
                CHECK_THROWS((void)resolver->resolve(uri));
            }
        }
    }
}
