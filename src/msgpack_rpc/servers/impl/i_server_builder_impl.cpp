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

#include "msgpack_rpc/servers/impl/server_builder_impl.h"

namespace msgpack_rpc::servers::impl {

std::unique_ptr<IServerBuilderImpl> create_empty_server_builder_impl(
    std::shared_ptr<executors::IExecutor> executor,
    std::shared_ptr<logging::Logger> logger) {
    return std::make_unique<ServerBuilderImpl>(
        std::move(executor), std::move(logger));
}

}  // namespace msgpack_rpc::servers::impl
