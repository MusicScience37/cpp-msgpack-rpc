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
 * \brief Test of ServerBuilderImpl class.
 */
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "../../create_test_logger.h"
#include "../../methods/mock_method.h"
#include "../../transport/mock_acceptor.h"
#include "../../transport/mock_acceptor_factory.h"
#include "../../transport/mock_backend.h"
#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/wrap_executor.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/methods/i_method.h"
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"
#include "msgpack_rpc/servers/impl/i_server_impl.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "trompeloeil_catch2.h"

TEST_CASE("msgpack_rpc::servers::impl::ServerBuilderImpl") {
    using msgpack_rpc::addresses::TCPAddress;
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::servers::impl::create_empty_server_builder_impl;
    using msgpack_rpc::servers::impl::IServerImpl;
    using msgpack_rpc::transport::IAcceptor;
    using msgpack_rpc_test::MockAcceptor;
    using msgpack_rpc_test::MockAcceptorFactory;
    using msgpack_rpc_test::MockBackend;
    using msgpack_rpc_test::MockMethod;
    using trompeloeil::_;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor = msgpack_rpc::executors::wrap_executor(
        msgpack_rpc::executors::create_single_thread_executor(logger));

    const auto backend = std::make_shared<MockBackend>();
    const auto scheme = std::string_view("tcp");
    ALLOW_CALL(*backend, scheme()).RETURN(scheme);

    const auto builder_impl =
        create_empty_server_builder_impl(executor, logger);
    CHECK_NOTHROW(builder_impl->register_protocol(backend));

    // Successful cases are tested in integration tests.

    SECTION("try to create a server without a URI to listen to") {
        auto method = std::make_unique<MockMethod>();
        auto& method_ref = *method;
        const auto method_name = MethodNameView("test_method");
        ALLOW_CALL(method_ref, name()).RETURN(method_name);
        builder_impl->add_method(std::move(method));

        CHECK_THROWS((void)builder_impl->build());
    }

    SECTION("try to create a server with a URI which results in no acceptors") {
        const std::string host = "localhost";
        constexpr std::uint16_t port = 0;
        builder_impl->listen_to(URI(scheme, host, port));

        auto method = std::make_unique<MockMethod>();
        auto& method_ref = *method;
        const auto method_name = MethodNameView("test_method");
        ALLOW_CALL(method_ref, name()).RETURN(method_name);
        builder_impl->add_method(std::move(method));

        const auto acceptor_factory = std::make_shared<MockAcceptorFactory>();
        REQUIRE_CALL(*acceptor_factory, create(_))
            .TIMES(1)
            .RETURN(std::vector<std::shared_ptr<IAcceptor>>{});
        REQUIRE_CALL(*backend, create_acceptor_factory())
            .TIMES(1)
            .RETURN(acceptor_factory);

        CHECK_THROWS((void)builder_impl->build());
    }
}
