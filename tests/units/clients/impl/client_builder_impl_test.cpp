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
 * \brief Test of ClientBuilderImpl class.
 */
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "../../create_test_logger.h"
#include "../../transport/mock_backend.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/impl/i_client_builder_impl.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/wrap_executor.h"
#include "trompeloeil_catch2.h"

TEST_CASE("msgpack_rpc::servers::impl::ClientBuilderImpl") {
    using msgpack_rpc::addresses::URI;
    using msgpack_rpc::clients::impl::create_empty_client_builder_impl;
    using msgpack_rpc::clients::impl::IClientImpl;
    using msgpack_rpc::config::ClientConfig;
    using msgpack_rpc_test::MockBackend;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);
    const auto async_executor = msgpack_rpc::executors::wrap_executor(executor);

    const auto backend = std::make_shared<MockBackend>();
    const auto scheme = std::string_view("tcp");
    ALLOW_CALL(*backend, scheme()).RETURN(scheme);

    const auto builder_impl = create_empty_client_builder_impl(
        async_executor, logger, ClientConfig());
    CHECK_NOTHROW(builder_impl->register_protocol(backend));

    SECTION("create a client") {
        const std::string host = "localhost";
        const std::string ip_address = "127.0.0.1";
        constexpr std::uint16_t port = 0;
        builder_impl->connect_to(URI(scheme, host, port));

        std::shared_ptr<IClientImpl> client_impl = builder_impl->build();
    }
}
