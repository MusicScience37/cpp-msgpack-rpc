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
#include <variant>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/clients/impl/call_list.h"
#include "msgpack_rpc/clients/impl/client_connector.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/clients/impl/sent_message_queue.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc/messages/serialized_message.h"
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
     * \param[in] call_list List of RPCs.
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     */
    ClientImpl(std::shared_ptr<ClientConnector> connector,
        std::shared_ptr<CallList> call_list,
        std::shared_ptr<executors::IAsyncExecutor> executor,
        std::shared_ptr<logging::Logger> logger)
        : connector_(std::move(connector)),
          call_list_(std::move(call_list)),
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
            [weak_self = this->weak_from_this()](
                const messages::ParsedMessage& message) {
                const auto self = weak_self.lock();
                if (self) {
                    self->on_received(message);
                }
            },
            // on_sent
            [weak_self = this->weak_from_this()] {
                const auto self = weak_self.lock();
                if (self) {
                    self->on_sent();
                }
            },
            // on_closed
            [weak_self = this->weak_from_this()] {
                const auto self = weak_self.lock();
                if (self) {
                    self->on_connection_closed();
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
        const auto call = call_list_->create(method_name, parameters);

        sent_messages_.push(call.serialized_request(), call.id());
        send_next();

        MSGPACK_RPC_DEBUG(
            logger_, "Send request {} (id: {})", method_name, call.id());

        return call.future();
    }

    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::notify
    void notify(messages::MethodNameView method_name,
        const IParametersSerializer& parameters) override {
        const auto serialized_notification =
            std::make_shared<messages::SerializedMessage>(
                parameters.create_serialized_notification(method_name));

        sent_messages_.push(serialized_notification);
        send_next();

        MSGPACK_RPC_DEBUG(logger_, "Send notification {}", method_name);
    }

private:
    /*!
     * \brief Send the next message if possible.
     */
    void send_next() {
        if (is_sending_.load(std::memory_order_relaxed)) {
            MSGPACK_RPC_TRACE(logger_, "Another message is being sent.");
            return;
        }
        const auto connection = connector_->connection();
        if (!connection) {
            MSGPACK_RPC_TRACE(
                logger_, "No connection now, so wait for connection.");
            return;
        }
        const auto [message, request_id] = sent_messages_.next();
        if (!message) {
            MSGPACK_RPC_TRACE(logger_, "No message to be sent for now.");
            return;
        }
        if (is_sending_.exchange(true, std::memory_order_acquire)) {
            MSGPACK_RPC_TRACE(logger_, "Another message is being sent.");
            return;
        }

        connection->async_send(message);
        MSGPACK_RPC_TRACE(logger_, "Sending next message.");
    }

    /*!
     * \brief Handle a message sent.
     */
    void on_sent() {
        MSGPACK_RPC_TRACE(logger_, "A message has been sent.");
        sent_messages_.pop();
        is_sending_.store(false, std::memory_order_release);
        send_next();
    }

    /*!
     * \brief Handle a received message.
     *
     * \param[in] message Message.
     */
    void on_received(const messages::ParsedMessage& message) {
        const auto* response = std::get_if<messages::ParsedResponse>(&message);
        if (response == nullptr) {
            MSGPACK_RPC_WARN(logger_, "Received an invalid message.");
            return;
        }
        MSGPACK_RPC_DEBUG(
            logger_, "Received response (id: {})", response->id());
        call_list_->handle(*response);
    }

    /*!
     * \brief Handle a connection closed.
     */
    void on_connection_closed() {
        MSGPACK_RPC_TRACE(logger_, "Connection closed, so reconnecting.");
        is_sending_.store(false, std::memory_order_release);
    }

    //! Connector.
    std::shared_ptr<ClientConnector> connector_;

    //! List of RPCs.
    std::shared_ptr<CallList> call_list_;

    //! Queue of messages to be sent.
    SentMessageQueue sent_messages_{};

    //! Executor.
    std::shared_ptr<executors::IAsyncExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Whether this client has been started.
    std::atomic<bool> is_started_{false};

    //! Whether this client has been stopped.
    std::atomic<bool> is_stopped_{false};

    //! Whether this client is sending a message.
    std::atomic<bool> is_sending_{false};
};

}  // namespace msgpack_rpc::clients::impl
