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
 * \brief Definition of ServerImpl class.
 */
#pragma once

#include <atomic>
#include <exception>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "msgpack_rpc/addresses/i_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/methods/i_method_processor.h"
#include "msgpack_rpc/servers/impl/i_server_impl.h"
#include "msgpack_rpc/servers/server_connection.h"
#include "msgpack_rpc/servers/stop_signal_handler.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::servers::impl {

/*!
 * \brief Class of internal implementation of servers.
 */
class ServerImpl final : public IServerImpl {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] acceptors Acceptors.
     * \param[in] processor Processor of methods.
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     */
    ServerImpl(std::vector<std::shared_ptr<transport::IAcceptor>> acceptors,
        std::unique_ptr<methods::IMethodProcessor> processor,
        std::shared_ptr<executors::IAsyncExecutor> executor,
        std::shared_ptr<logging::Logger> logger)
        : acceptors_(std::move(acceptors)),
          processor_(std::move(processor)),
          executor_(std::move(executor)),
          logger_(std::move(logger)),
          stop_signal_handler_(std::make_shared<StopSignalHandler>(logger_)) {}

    //! Destructor.
    ~ServerImpl() override {
        try {
            stop();
        } catch (const std::exception& e) {
            MSGPACK_RPC_CRITICAL(logger_,
                "An exception was thrown when destructing a server but "
                "ignored: "
                "{}",
                e.what());
        }
    }

    ServerImpl(const ServerImpl&) = delete;
    ServerImpl(ServerImpl&&) = delete;
    ServerImpl& operator=(const ServerImpl&) = delete;
    ServerImpl& operator=(ServerImpl&&) = delete;

    /*!
     * \brief Start processing of this server.
     */
    void start() {
        if (is_started_.exchange(true)) {
            throw MsgpackRPCException(StatusCode::PRECONDITION_NOT_MET,
                "This server has already been started.");
        }
        start_acceptors();

        executor_->on_exception(
            [weak_signal_handler =
                    std::weak_ptr<StopSignalHandler>(stop_signal_handler_)](
                const std::exception_ptr& /*exception*/) {
                const auto signal_handler = weak_signal_handler.lock();
                if (signal_handler) {
                    signal_handler->stop();
                }
            });

        executor_->start();
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerImpl::stop
    void stop() override {
        if (!is_started_.load()) {
            return;
        }
        if (is_stopped_.exchange(true)) {
            return;
        }
        stop_acceptors();
        acceptors_.clear();
        processor_.reset();
        executor_->stop();
        executor_.reset();
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerImpl::run_until_signal
    void run_until_signal() override {
        stop_signal_handler_->wait();

        const auto last_exception = executor_->last_exception();

        stop();

        if (last_exception) {
            std::rethrow_exception(last_exception);
        }
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerImpl::local_endpoint_uris
    [[nodiscard]] std::vector<addresses::URI> local_endpoint_uris() override {
        std::vector<addresses::URI> uris;
        uris.reserve(acceptors_.size());
        for (const auto& acceptor : acceptors_) {
            uris.push_back(acceptor->local_address().to_uri());
        }
        return uris;
    }

    //! \copydoc msgpack_rpc::servers::impl::IServerImpl::executor
    [[nodiscard]] std::shared_ptr<executors::IExecutor> executor() override {
        return executor_;
    }

private:
    /*!
     * \brief Start processing of acceptors.
     */
    void start_acceptors() {
        for (const auto& acceptor : acceptors_) {
            acceptor->start(
                [executor = std::weak_ptr<executors::IExecutor>(executor_),
                    processor = processor_, logger = logger_](
                    const std::shared_ptr<transport::IConnection>& connection) {
                    const auto handler = std::make_shared<ServerConnection>(
                        connection, executor, processor, logger);
                    handler->start();
                });
            MSGPACK_RPC_DEBUG(logger_, "Listening to {}.",
                acceptor->local_address().to_string());
        }
    }

    /*!
     * \brief Stop processing of acceptors.
     */
    void stop_acceptors() {
        for (const auto& acceptor : acceptors_) {
            acceptor->stop();
        }
    }

    //! Acceptors.
    std::vector<std::shared_ptr<transport::IAcceptor>> acceptors_;

    //! Processor of methods.
    std::shared_ptr<methods::IMethodProcessor> processor_;

    //! Executor.
    std::shared_ptr<executors::IAsyncExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Handler of signals to stop processing.
    std::shared_ptr<StopSignalHandler> stop_signal_handler_;

    //! Whether this server has been started.
    std::atomic<bool> is_started_{false};

    //! Whether this server has been stopped.
    std::atomic<bool> is_stopped_{false};
};

}  // namespace msgpack_rpc::servers::impl
