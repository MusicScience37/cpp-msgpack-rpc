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
 * \brief Test of ServerBuilder class.
 */
#include "msgpack_rpc/servers/server_builder.h"

#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

#include "../create_test_logger.h"
#include "../transport/mock_acceptor.h"
#include "../transport/mock_acceptor_factory.h"
#include "../transport/mock_backend.h"
#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/wrap_executor.h"
#include "msgpack_rpc/methods/method_exception.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "trompeloeil_catch2.h"

TEST_CASE("msgpack_rpc::servers::ServerBuilder") {
    using msgpack_rpc::addresses::TCPAddress;
    using msgpack_rpc::servers::IServer;
    using msgpack_rpc::servers::ServerBuilder;
    using msgpack_rpc::servers::impl::create_empty_server_builder_impl;
    using msgpack_rpc::transport::IAcceptor;
    using msgpack_rpc_test::MockAcceptor;
    using msgpack_rpc_test::MockAcceptorFactory;
    using msgpack_rpc_test::MockBackend;
    using trompeloeil::_;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor = msgpack_rpc::executors::wrap_executor(
        msgpack_rpc::executors::create_single_thread_executor(logger));

    const auto backend = std::make_shared<MockBackend>();
    const auto scheme = std::string_view("tcp");
    ALLOW_CALL(*backend, scheme()).RETURN(scheme);

    auto builder_impl = create_empty_server_builder_impl(executor, logger);
    REQUIRE_NOTHROW(builder_impl->register_protocol(backend));
    ServerBuilder builder{std::move(builder_impl)};

    SECTION("create a server") {
        const std::string host = "localhost";
        const std::string ip_address = "127.0.0.1";
        constexpr std::uint16_t port = 0;
        REQUIRE_NOTHROW(builder.listen_to_tcp(host, port));

        REQUIRE_NOTHROW(builder.add_method<int(int, int)>(
            "add", [](int a, int b) { return a + b; }));

        const auto acceptor_factory = std::make_shared<MockAcceptorFactory>();
        const auto acceptor = std::make_shared<MockAcceptor>();
        REQUIRE_CALL(*acceptor_factory, create(_))
            .TIMES(1)
            .RETURN(std::vector<std::shared_ptr<IAcceptor>>{acceptor});
        REQUIRE_CALL(*backend, create_acceptor_factory())
            .TIMES(1)
            .RETURN(acceptor_factory);

        std::unique_ptr<IServer> server = builder.build();
    }
}
