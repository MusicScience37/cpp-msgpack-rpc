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
#include <exception>
#include <memory>
#include <optional>
#include <utility>

#include "msgpack_rpc/clients/impl/call_list.h"
#include "msgpack_rpc/clients/impl/client_connector.h"
#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/clients/impl/message_sender.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/clients/impl/received_message_processor.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of internal implementation of clients.
 */
class ClientImpl final : public IClientImpl {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] connector Connector.
     * \param[in] call_list List of RPCs.
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     */
    ClientImpl(std::shared_ptr<ClientConnector> connector,
        std::shared_ptr<CallList> call_list,
        std::shared_ptr<executors::IAsyncExecutor> executor,
        std::shared_ptr<logging::Logger> logger)
        : executor_(std::move(executor)),
          connector_(std::move(connector)),
          call_list_(std::move(call_list)),
          logger_(std::move(logger)),
          sender_(std::make_shared<MessageSender>(connector_, logger_)) {}

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

    /*!
     * \brief Start processing of this client.
     */
    void start() {
        if (is_started_.exchange(true)) {
            throw MsgpackRPCException(StatusCode::PRECONDITION_NOT_MET,
                "This client has already been started.");
        }
        connector_->start(
            // on_connection
            [sender = sender_] { sender->send_next(); },
            // on_received
            ReceivedMessageProcessor(logger_, call_list_),
            // on_sent
            [sender = sender_] {
                sender->handle_sent_message();
                sender->send_next();
            },
            // on_closed
            [sender = sender_] { sender->handle_disconnection(); });
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
        connector_.reset();
        call_list_.reset();
        sender_.reset();
        executor_->stop();
        executor_.reset();
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::async_call
    [[nodiscard]] std::shared_ptr<ICallFutureImpl> async_call(
        messages::MethodNameView method_name,
        const IParametersSerializer& parameters) override {
        check_executor_state();

        const auto [request_id, serialized_request, future] =
            call_list_->create(method_name, parameters);

        sender_->send(serialized_request, request_id);

        MSGPACK_RPC_DEBUG(
            logger_, "Send request {} (id: {})", method_name, request_id);

        return future;
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::notify
    void notify(messages::MethodNameView method_name,
        const IParametersSerializer& parameters) override {
        check_executor_state();

        const auto serialized_notification =
            parameters.create_serialized_notification(method_name);

        sender_->send(serialized_notification);

        MSGPACK_RPC_DEBUG(logger_, "Send notification {}", method_name);
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::executor
    [[nodiscard]] std::shared_ptr<executors::IExecutor> executor() override {
        return executor_;
    }

private:
    /*!
     * \brief Check whether the executor is running.
     */
    void check_executor_state() {
        if (!executor_ || !executor_->is_running()) {
            throw MsgpackRPCException(StatusCode::PRECONDITION_NOT_MET,
                "This client has been stopped.");
        }
    }

    //! Executor.
    std::shared_ptr<executors::IAsyncExecutor> executor_;

    //! Connector.
    std::shared_ptr<ClientConnector> connector_;

    //! List of RPCs.
    std::shared_ptr<CallList> call_list_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Sender of messages.
    std::shared_ptr<MessageSender> sender_;

    //! Whether this client has been started.
    std::atomic<bool> is_started_{false};

    //! Whether this client has been stopped.
    std::atomic<bool> is_stopped_{false};
};

}  // namespace msgpack_rpc::clients::impl
