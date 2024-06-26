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
 * \brief Definition of IServerBuilderImpl class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/methods/i_method.h"
#include "msgpack_rpc/servers/impl/i_server_impl.h"
#include "msgpack_rpc/transport/i_backend.h"

namespace msgpack_rpc::servers::impl {

/*!
 * \brief Interface of implementation of builders of servers.
 */
class IServerBuilderImpl {
public:
    /*!
     * \brief Register a protocol.
     *
     * \param[in] backend Backend of the protocol.
     */
    virtual void register_protocol(
        std::shared_ptr<transport::IBackend> backend) = 0;

    /*!
     * \brief Add a URI to listen to.
     *
     * \param[in] uri URI.
     */
    virtual void listen_to(addresses::URI uri) = 0;

    /*!
     * \brief Add a method.
     *
     * \param[in] method Method.
     */
    virtual void add_method(std::unique_ptr<methods::IMethod> method) = 0;

    /*!
     * \brief Build a server.
     *
     * \return Server.
     */
    [[nodiscard]] virtual std::unique_ptr<IServerImpl> build() = 0;

    /*!
     * \brief Get the logger in this builder.
     *
     * \return Logger.
     */
    [[nodiscard]] virtual std::shared_ptr<logging::Logger> logger() = 0;

    IServerBuilderImpl(const IServerBuilderImpl&) = delete;
    IServerBuilderImpl(IServerBuilderImpl&&) = delete;
    IServerBuilderImpl& operator=(const IServerBuilderImpl&) = delete;
    IServerBuilderImpl& operator=(IServerBuilderImpl&&) = delete;

    //! Destructor.
    virtual ~IServerBuilderImpl() noexcept = default;

protected:
    //! Constructor.
    IServerBuilderImpl() noexcept = default;
};

/*!
 * \brief Create an empty IServerBuilderImpl object.
 *
 * \param[in] executor Executor.
 * \param[in] logger Logger.
 * \return IServerBuilderImpl object.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::unique_ptr<IServerBuilderImpl>
create_empty_server_builder_impl(
    std::shared_ptr<executors::IAsyncExecutor> executor,
    std::shared_ptr<logging::Logger> logger);

/*!
 * \brief Create an IServerBuilderImpl object with default protocols.
 *
 * \param[in] server_config Configuration of the server.
 * \param[in] logger Logger.
 * \return IServerBuilderImpl object.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::unique_ptr<IServerBuilderImpl>
create_default_builder_impl(const config::ServerConfig& server_config,
    const std::shared_ptr<logging::Logger>& logger);

}  // namespace msgpack_rpc::servers::impl
