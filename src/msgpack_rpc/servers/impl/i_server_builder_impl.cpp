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
 * \brief Definition of a function for IServerBuilderImpl class.
 */
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"

#include <memory>
#include <utility>
#include <vector>

#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/servers/impl/server_builder_impl.h"
#include "msgpack_rpc/transport/backends.h"

namespace msgpack_rpc::servers::impl {

std::unique_ptr<IServerBuilderImpl> create_empty_server_builder_impl(
    std::shared_ptr<executors::IAsyncExecutor> executor,
    std::shared_ptr<logging::Logger> logger) {
    return std::make_unique<ServerBuilderImpl>(
        std::move(executor), std::move(logger));
}

std::unique_ptr<IServerBuilderImpl> create_default_builder_impl(
    const config::ServerConfig& server_config,
    const std::shared_ptr<logging::Logger>& logger) {
    const auto executor =
        executors::create_executor(logger, server_config.executor());

    auto builder = create_empty_server_builder_impl(executor, logger);

    builder->register_protocol(transport::create_tcp_backend(
        executor, server_config.message_parser(), logger));

    for (const auto& uri : server_config.uris()) {
        builder->listen_to(uri);
    }

    return builder;
}

}  // namespace msgpack_rpc::servers::impl
