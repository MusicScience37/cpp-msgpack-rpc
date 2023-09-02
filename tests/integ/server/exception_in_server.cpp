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
 * \brief Test of an exception in a server.
 */
#include <future>
#include <memory>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>

#include "check_connectivity.h"
#include "create_test_logger.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/servers/server_builder.h"

SCENARIO("Exception in a server") {
    using msgpack_rpc::servers::ServerBuilder;

    const auto logger = msgpack_rpc_test::create_test_logger();

    GIVEN("A server") {
        auto server =
            ServerBuilder{logger}.listen_to("tcp://localhost:0").build();
        auto server_future =
            std::async([&server] { server->run_until_signal(); });

        const auto uris = server->local_endpoint_uris();
        CHECK_NOTHROW(check_connectivity(uris));

        WHEN("An exception is thrown in a thread of the server") {
            msgpack_rpc::executors::async_invoke(server->executor(),
                msgpack_rpc::executors::OperationType::CALLBACK, [] {
                    throw std::runtime_error(
                        "Test exception in a thread of the server.");
                });

            THEN("The server stops") { CHECK_THROWS(server_future.get()); }
        }
    }
}
