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
 * \brief Definition of IClientBuilderImpl class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/i_backend.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Interface of implementation of builders of clients.
 */
class IClientBuilderImpl {
public:
    /*!
     * \brief Register a protocol.
     *
     * \param[in] backend Backend of the protocol.
     */
    virtual void register_protocol(
        std::shared_ptr<transport::IBackend> backend) = 0;

    /*!
     * \brief Add a URI to connect to.
     *
     * \param[in] uri URI.
     */
    virtual void connect_to(addresses::URI uri) = 0;

    /*!
     * \brief Build a client.
     *
     * \return Client.
     */
    [[nodiscard]] virtual std::shared_ptr<clients::impl::IClientImpl>
    build() = 0;

    IClientBuilderImpl(const IClientBuilderImpl&) = delete;
    IClientBuilderImpl(IClientBuilderImpl&&) = delete;
    IClientBuilderImpl& operator=(const IClientBuilderImpl&) = delete;
    IClientBuilderImpl& operator=(IClientBuilderImpl&&) = delete;

    //! Destructor.
    virtual ~IClientBuilderImpl() noexcept = default;

protected:
    //! Constructor.
    IClientBuilderImpl() noexcept = default;
};

/*!
 * \brief Create an empty IClientBuilderImpl object.
 *
 * \param[in] executor Executor.
 * \param[in] logger Logger.
 * \param[in] config Configuration.
 * \return IClientBuilderImpl object.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::unique_ptr<IClientBuilderImpl>
create_empty_client_builder_impl(
    std::shared_ptr<executors::IAsyncExecutor> executor,
    std::shared_ptr<logging::Logger> logger, config::ClientConfig config);

/*!
 * \brief Create an IClientBuilderImpl object with default protocols.
 *
 * \param[in] config Configuration.
 * \param[in] logger Logger.
 * \return IClientBuilderImpl object.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::unique_ptr<IClientBuilderImpl>
create_default_client_builder_impl(config::ClientConfig config,
    const std::shared_ptr<logging::Logger>& logger);

};  // namespace msgpack_rpc::clients::impl
