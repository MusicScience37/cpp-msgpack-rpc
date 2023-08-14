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
 * \brief Implementation of ClientBuilderImpl class.
 */
#pragma once

#include <chrono>
#include <memory>
#include <utility>

#include "msgpack_rpc/clients/impl/call_list.h"
#include "msgpack_rpc/clients/impl/client_connector.h"
#include "msgpack_rpc/clients/impl/client_impl.h"
#include "msgpack_rpc/clients/impl/i_client_builder_impl.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/backend_list.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of implementation of builders of clients.
 */
class ClientBuilderImpl final : public IClientBuilderImpl {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     * \param[in] config Configuration.
     */
    ClientBuilderImpl(std::shared_ptr<executors::IAsyncExecutor> executor,
        std::shared_ptr<logging::Logger> logger, config::ClientConfig config)
        : executor_(std::move(executor)),
          logger_(std::move(logger)),
          config_(std::move(config)) {}

    //! \copydoc msgpack_rpc::clients::impl::IClientBuilderImpl::register_protocol
    void register_protocol(
        std::shared_ptr<transport::IBackend> backend) override {
        backends_.append(backend);
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientBuilderImpl::connect_to
    void connect_to(const addresses::URI& uri) override {
        config_.add_uri(uri);
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientBuilderImpl::build
    [[nodiscard]] std::shared_ptr<clients::impl::IClientImpl> build() override {
        const auto connector = std::make_shared<ClientConnector>(executor_,
            backends_, config_.uris(), config_.reconnection(), logger_);

        const auto timeout = std::chrono::milliseconds(1);  // TODO
        const auto call_list =
            std::make_shared<CallList>(timeout, executor_, logger_);

        return std::make_shared<ClientImpl>(
            connector, call_list, executor_, logger_);
    }

private:
    //! Executor.
    std::shared_ptr<executors::IAsyncExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Configuration.
    config::ClientConfig config_;

    //! Backends.
    transport::BackendList backends_{};
};

}  // namespace msgpack_rpc::clients::impl
