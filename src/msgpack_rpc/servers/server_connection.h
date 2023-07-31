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
 * \brief Definition of Server class.
 */
#pragma once

#include <atomic>
#include <exception>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <asio/post.hpp>
#include <fmt/format.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/i_method_processor.h"
#include "msgpack_rpc/servers/i_server.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::servers {

/*!
 * \brief Class to handle connections in servers.
 */
class ServerConnection : public std::enable_shared_from_this<ServerConnection> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] connection Connection.
     * \param[in] executor Executor.
     * \param[in] processor Processor of methods.
     * \param[in] logger Logger.
     */
    ServerConnection(const std::shared_ptr<transport::IConnection>& connection,
        std::weak_ptr<executors::IExecutor> executor,
        std::shared_ptr<methods::IMethodProcessor> processor,
        std::shared_ptr<logging::Logger> logger)
        : connection_(connection),
          executor_(std::move(executor)),
          processor_(std::move(processor)),
          logger_(std::move(logger)),
          formatted_remote_address_(
              fmt::format("{}", connection->remote_address())) {}

    /*!
     * \brief Start processing.
     */
    void start() {
        const auto connection = connection_.lock();
        if (connection) {
            connection->start(
                [self = this->shared_from_this()](
                    messages::ParsedMessage message) {
                    self->on_received(std::move(message));
                },
                [self = this->shared_from_this()]() { self->on_sent(); },
                [](const Status& /*status*/) {
                    // No operation when this connection is closed.
                });
        }
    }

private:
    /*!
     * \brief Process a received message.
     *
     * \param[in] message Message.
     */
    void on_received(messages::ParsedMessage message) {
        const auto executor = executor_.lock();
        if (!executor) {
            return;
        }

        std::visit(
            [this, executor](auto&& concrete_message) {
                if constexpr (std::is_same_v<messages::ParsedRequest,
                                  std::decay_t<decltype(concrete_message)>>) {
                    asio::post(
                        executor->context(executors::OperationType::CALLBACK),
                        [self = this->shared_from_this(),
                            // NOLINTNEXTLINE(bugprone-move-forwarding-reference): This actually always moves rvalue.
                            request = std::move(concrete_message)] {
                            self->on_request(request);
                        });
                } else if constexpr (std::is_same_v<
                                         messages::ParsedNotification,
                                         std::decay_t<
                                             decltype(concrete_message)>>) {
                    asio::post(
                        executor->context(executors::OperationType::CALLBACK),
                        [self = this->shared_from_this(),
                            // NOLINTNEXTLINE(bugprone-move-forwarding-reference): This actually always moves rvalue.
                            notification = std::move(concrete_message)] {
                            self->on_notification(notification);
                        });
                } else {
                    this->on_invalid_message();
                }
            },
            std::move(message));
    }

    /*!
     * \brief Process a request.
     *
     * \param[in] request Request.
     */
    void on_request(const messages::ParsedRequest& request) {
        MSGPACK_RPC_DEBUG(logger_, "{} request {} (id: {})",
            formatted_remote_address_, request.method_name(), request.id());

        auto serialized_response =
            std::make_shared<messages::SerializedMessage>(
                processor_->call(request));

        MSGPACK_RPC_DEBUG(logger_, "{} respond {} (id: {})",
            formatted_remote_address_, request.method_name(), request.id());

        {
            std::unique_lock<std::mutex> lock(message_queue_mutex_);
            message_queue_.push(std::move(serialized_response));
        }

        send_next_if_exists();
    }

    /*!
     * \brief Process a notification.
     *
     * \param[in] notification Notification.
     */
    void on_notification(const messages::ParsedNotification& notification) {
        MSGPACK_RPC_DEBUG(logger_, "{} notify {}", formatted_remote_address_,
            notification.method_name());

        processor_->notify(notification);
    }

    /*!
     * \brief Handle an invalid message.
     */
    void on_invalid_message() {
        MSGPACK_RPC_DEBUG(logger_,
            "Unexpectedly received a response from {}, so close the "
            "connection.",
            formatted_remote_address_);
        const auto connection = connection_.lock();
        if (connection) {
            connection->async_close();
        }
    }

    /*!
     * \brief Send the next message if exists.
     */
    void send_next_if_exists() {
        std::unique_lock<std::mutex> lock(message_queue_mutex_);
        if (message_queue_.empty()) {
            return;
        }
        const auto next_message = std::move(message_queue_.front());
        message_queue_.pop();
        lock.unlock();

        const auto connection = connection_.lock();
        if (connection) {
            connection->async_send(next_message);
        }
    }

    /*!
     * \brief Handle the condition that a message is sent.
     */
    void on_sent() { send_next_if_exists(); }

    //! Connection.
    std::weak_ptr<transport::IConnection> connection_;

    //! Executor.
    std::weak_ptr<executors::IExecutor> executor_;

    //! Processor of methods.
    std::shared_ptr<methods::IMethodProcessor> processor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Formatted remote address for logging.
    std::string formatted_remote_address_;

    //! Messages to be sent.
    std::queue<std::shared_ptr<const messages::SerializedMessage>>
        message_queue_{};

    //! Mutex of message_queue_.
    std::mutex message_queue_mutex_{};
};

}  // namespace msgpack_rpc::servers
