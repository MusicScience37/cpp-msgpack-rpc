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
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/methods/i_method_processor.h"
#include "msgpack_rpc/methods/method_processor.h"
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"
#include "msgpack_rpc/servers/server.h"
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
     */
    ServerBuilderImpl(std::shared_ptr<executors::IExecutor> executor,
        std::shared_ptr<logging::Logger> logger)
        : executor_(std::move(executor)),
          logger_(std::move(logger)),
          processor_(methods::create_method_processor(logger_)) {}

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::register_protocol
    void register_protocol(
        std::shared_ptr<transport::IBackend> backend) override {
        const auto scheme = backend->scheme();
        backends_.try_emplace(scheme, std::move(backend));
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::listen_to
    void listen_to(addresses::Address address) override {
        addresses_.push_back(std::move(address));
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::add_method
    void add_method(std::unique_ptr<methods::IMethod> method) override {
        processor_->append(std::move(method));
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerBuilderImpl::build
    [[nodiscard]] std::unique_ptr<IServer> build() override {
        std::vector<std::shared_ptr<transport::IAcceptor>> acceptors;
        for (const auto& address : addresses_) {
            // TODO resolve from URIs.
            const auto backend_iter = backends_.find(address.to_uri().scheme());
            if (backend_iter == backends_.end()) {
                throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
                    fmt::format(
                        "Invalid scheme: {}.", address.to_uri().scheme()));
            }
            auto acceptor = backend_iter->second->create_acceptor(address);
            acceptors.push_back(std::move(acceptor));
        }
        return std::make_unique<Server>(
            std::move(acceptors), std::move(processor_), executor_, logger_);
    }

private:
    //! Executor.
    std::shared_ptr<executors::IExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Backends.
    std::unordered_map<std::string_view, std::shared_ptr<transport::IBackend>>
        backends_{};

    //! Addresses to listen to.
    std::vector<addresses::Address> addresses_{};

    //! Processor of methods.
    std::unique_ptr<methods::IMethodProcessor> processor_;
};

}  // namespace msgpack_rpc::servers::impl
