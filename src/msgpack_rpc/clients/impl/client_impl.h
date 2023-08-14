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
 * \brief Definition of ClientImpl class.
 */
#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/clients/impl/client_connector.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of internal implementation of clients.
 */
class ClientImpl final : public IClientImpl,
                         public std::enable_shared_from_this<ClientImpl> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] connector Connector.
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     */
    ClientImpl(std::shared_ptr<ClientConnector> connector,
        std::shared_ptr<executors::IAsyncExecutor> executor,
        std::shared_ptr<logging::Logger> logger)
        : connector_(std::move(connector)),
          executor_(std::move(executor)),
          logger_(std::move(logger)) {}

    /*!
     * \brief Destructor.
     */
    ~ClientImpl() override {
        try {
            stop();
        } catch (const std::exception& e) {
            MSGPACK_RPC_CRITICAL(logger_,
                "An exception was thrown when destructing a client but "
                "ignored: {}",
                e.what());
        }
    }

    ClientImpl(const ClientImpl&) = delete;
    ClientImpl(ClientImpl&&) = delete;
    ClientImpl& operator=(const ClientImpl&) = delete;
    ClientImpl& operator=(ClientImpl&&) = delete;

    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::start
    void start() override {
        if (is_started_.exchange(true)) {
            throw MsgpackRPCException(StatusCode::PRECONDITION_NOT_MET,
                "This client has already been started.");
        }
        connector_->start(
            // on_connection
            [weak_self = this->weak_from_this()] {
                const auto self = weak_self.lock();
                if (self) {
                    self->send_next();
                }
            },
            // on_received
            [](const messages::ParsedMessage& message) {
                // TODO
                (void)message;
            },
            // on_sent
            [weak_self = this->weak_from_this()] {
                const auto self = weak_self.lock();
                if (self) {
                    self->send_next();
                }
            });
        executor_->start();
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::stop
    void stop() override {
        if (!is_started_.load()) {
            return;
        }
        if (is_stopped_.exchange(true)) {
            return;
        }
        connector_->stop();
        executor_->stop();
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::async_call
    [[nodiscard]] std::shared_ptr<ICallFutureImpl> async_call(
        messages::MethodNameView method_name,
        const IParametersSerializer& parameters) override {
        // TODO
        (void)method_name;
        (void)parameters;
        return std::make_shared<CallFutureImpl>();
    }

private:
    /*!
     * \brief Send the next message if possible.
     */
    void send_next() {
        // TODO
    }

    //! States of sending process.
    enum class SendingState {
        //! No connection exists.
        NO_CONNECTION,

        //! Connecting to a server.
        CONNECTING,

        //! Idle state with a connection but without a sending message.
        IDLE,

        //! Sending a message.
        SENDING
    };

    //! Connector.
    std::shared_ptr<ClientConnector> connector_;

    //! Executor.
    std::shared_ptr<executors::IAsyncExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Whether this server has been started.
    std::atomic<bool> is_started_{false};

    //! Whether this server has been stopped.
    std::atomic<bool> is_stopped_{false};
};

}  // namespace msgpack_rpc::clients::impl
