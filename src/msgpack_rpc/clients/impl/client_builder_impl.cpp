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
 * \brief Implementation of functions for IClientBuilderImpl class.
 */
#include "msgpack_rpc/clients/impl/client_builder_impl.h"

#include <memory>

#include "msgpack_rpc/clients/impl/i_client_builder_impl.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/create_default_backend_list.h"

namespace msgpack_rpc::clients::impl {

std::unique_ptr<IClientBuilderImpl> create_empty_client_builder_impl(
    std::shared_ptr<executors::IAsyncExecutor> executor,
    std::shared_ptr<logging::Logger> logger, config::ClientConfig config) {
    return std::make_unique<ClientBuilderImpl>(std::move(executor),
        std::move(logger), std::move(config), transport::BackendList());
}

std::unique_ptr<IClientBuilderImpl> create_default_client_builder_impl(
    config::ClientConfig config,
    const std::shared_ptr<logging::Logger>& logger) {
    const auto executor = executors::create_executor(logger, config.executor());

    auto builder =
        std::make_unique<ClientBuilderImpl>(executor, logger, std::move(config),
            transport::create_default_backend_list(
                executor, config.message_parser(), logger));

    return builder;
}

}  // namespace msgpack_rpc::clients::impl
