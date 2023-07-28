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

#include <memory>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "../create_test_logger.h"
#include "../transport/mock_backend.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"

// TODO
/*
TEST_CASE("msgpack_rpc::servers::ServerBuilder") {
    using msgpack_rpc::servers::ServerBuilder;
    using msgpack_rpc::servers::impl::create_empty_server_builder_impl;
    using msgpack_rpc_test::MockBackend;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);

    const auto backend = std::make_shared<MockBackend>();
    const auto scheme = std::string_view("tcp");
    ALLOW_CALL(*backend, scheme()).RETURN(scheme);

    auto builder_impl = create_empty_server_builder_impl(executor, logger);
    CHECK_NOTHROW(builder_impl->register_protocol(backend));
    ServerBuilder builder{std::move(builder_impl)};

    SECTION("create a server") {
        const std::string host = "127.0.0.1";
        constexpr std::uint16_t port = 0;
        builder.listen_to_tcp(host, port);
    }
}
*/
