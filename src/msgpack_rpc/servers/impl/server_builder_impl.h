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
 * \brief Definition of ServerBuilderImpl class.
 */
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/methods/i_method.h"
#include "msgpack_rpc/methods/i_method_processor.h"
#include "msgpack_rpc/methods/method_processor.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"
#include "msgpack_rpc/servers/server_impl.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_acceptor_factory.h"
#include "msgpack_rpc/transport/i_backend.h"

namespace msgpack_rpc::servers::impl {

/*!
 * \brief Class of implementation of builders of servers.
 */
class ServerBuilderImpl final : public IServerBuilderImpl {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     * \param[in] backends Backends.
     * \param[in] uris URIs to listen to.
     */
    ServerBuilderImpl(std::shared_ptr<executors::IAsyncExecutor> executor,
        std::shared_ptr<logging::Logger> logger,
        transport::BackendList backends, std::vector<addresses::URI> uris)
        : executor_(std::move(executor)),
          logger_(std::move(logger)),
          backends_(std::move(backends)),
          uris_(std::move(uris)),
          processor_(methods::create_method_processor(logger_)) {}

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::register_protocol
    void register_protocol(
        std::shared_ptr<transport::IBackend> backend) override {
        backends_.append(backend);
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::listen_to
    void listen_to(addresses::URI uri) override {
        uris_.push_back(std::move(uri));
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::add_method
    void add_method(std::unique_ptr<methods::IMethod> method) override {
        processor_->append(std::move(method));
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::build
    [[nodiscard]] std::unique_ptr<IServer> build() override {
        std::vector<std::shared_ptr<transport::IAcceptor>> acceptors;
        for (const auto& uri : uris_) {
            const auto backend = backends_.find(uri.scheme());

            const auto added_acceptors =
                backend->create_acceptor_factory()->create(uri);
            for (const auto& acceptor : added_acceptors) {
                acceptors.push_back(acceptor);
            }
        }
        return std::make_unique<ServerImpl>(
            std::move(acceptors), std::move(processor_), executor_, logger_);
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::logger
    [[nodiscard]] std::shared_ptr<logging::Logger> logger() override {
        return logger_;
    }

private:
    //! Executor.
    std::shared_ptr<executors::IAsyncExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Backends.
    transport::BackendList backends_{};

    //! URIs to listen to.
    std::vector<addresses::URI> uris_{};

    //! Processor of methods.
    std::unique_ptr<methods::IMethodProcessor> processor_;
};

}  // namespace msgpack_rpc::servers::impl
