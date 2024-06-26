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
 * \brief Definition of ClientConnector class.
 */
#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/impl/reconnection_timer.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/reconnection_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/async_connect.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class to establish connections for clients.
 */
class ClientConnector : public std::enable_shared_from_this<ClientConnector> {
public:
    //! Type of callback functions called when a connection is established.
    using ConnectionCallback = std::function<void()>;

    //! \copydoc msgpack_rpc::transport::IConnection::MessageReceivedCallback
    using MessageReceivedCallback =
        transport::IConnection::MessageReceivedCallback;

    //! \copydoc msgpack_rpc::transport::IConnection::MessageSentCallback
    using MessageSentCallback = transport::IConnection::MessageSentCallback;

    //! Type of callback functions called when a connection is closed.
    using ConnectionClosedCallback = std::function<void()>;

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] backends Backends.
     * \param[in] server_uris URIs of servers.
     * \param[in] reconnection_config Configuration of reconnection.
     * \param[in] logger Logger.
     */
    ClientConnector(const std::shared_ptr<executors::IExecutor>& executor,
        transport::BackendList backends,
        std::vector<addresses::URI> server_uris,
        const config::ReconnectionConfig& reconnection_config,
        const std::shared_ptr<logging::Logger>& logger)
        : backends_(std::move(backends)),
          server_uris_(std::move(server_uris)),
          retry_timer_(executor, logger, reconnection_config),
          logger_(logger) {}

    /*!
     * \brief Start processing.
     *
     * \param[in] on_connection Callback function called when a connection is
     * established.
     * \param[in] on_received Callback function called when a message is
     * received.
     * \param[in] on_sent Callback function called when a message is sent.
     * \param[in] on_closed Callback function called when a connection is
     * closed.
     */
    void start(ConnectionCallback on_connection,
        MessageReceivedCallback on_received, MessageSentCallback on_sent,
        ConnectionClosedCallback on_closed) {
        on_connection_ = std::move(on_connection);
        on_received_ = std::move(on_received);
        on_sent_ = std::move(on_sent);
        on_closed_ = std::move(on_closed);
        async_connect();
    }

    /*!
     * \brief Stop processing.
     */
    void stop() {
        if (is_stopped_.exchange(true, std::memory_order_acquire)) {
            return;
        }

        std::unique_lock<std::mutex> lock(connection_mutex_);
        if (connection_) {
            connection_->async_close();
            connection_.reset();
        }
        lock.unlock();
        retry_timer_.cancel();
    }

    /*!
     * \brief Get the connection.
     *
     * \return Connection. (Null if not connected.)
     */
    [[nodiscard]] std::shared_ptr<transport::IConnection> connection() {
        std::unique_lock<std::mutex> lock(connection_mutex_);
        return connection_;
    }

private:
    /*!
     * \brief Asynchronously connect to a server.
     */
    void async_connect() {
        transport::async_connect(backends_, server_uris_,
            [self = this->shared_from_this()](const Status& status,
                const std::shared_ptr<transport::IConnection>& connection) {
                if (status.code() == StatusCode::SUCCESS) {
                    self->on_connection(connection);
                } else {
                    self->on_connection_failure();
                }
            });
    }

    /*!
     * \brief Handle connection established.
     *
     * \param[in] connection Connection.
     */
    void on_connection(
        const std::shared_ptr<transport::IConnection>& connection) {
        if (is_stopped_.load(std::memory_order_relaxed)) {
            return;
        }
        std::unique_lock<std::mutex> lock(connection_mutex_);
        connection_ = connection;
        connection->start(on_received_, on_sent_,
            [weak_self = weak_from_this()](const Status& /*status*/) {
                const auto self = weak_self.lock();
                if (self) {
                    self->on_connection_closed();
                }
            });
        retry_timer_.reset();
        lock.unlock();

        on_connection_();
    }

    /*!
     * \brief Handle failure to connect.
     */
    void on_connection_failure() {
        if (is_stopped_.load(std::memory_order_relaxed)) {
            return;
        }
        retry_timer_.async_wait(
            [self = this->shared_from_this()] { self->async_connect(); });
    }

    /*!
     * \brief Handle connection closed.
     */
    void on_connection_closed() {
        if (is_stopped_.load(std::memory_order_relaxed)) {
            return;
        }
        std::unique_lock<std::mutex> lock(connection_mutex_);
        connection_.reset();
        lock.unlock();
        MSGPACK_RPC_TRACE(logger_, "Connection closed, so reconnect now.");
        on_closed_();
        async_connect();
    }

    //! Backends.
    transport::BackendList backends_;

    //! URIs of servers.
    std::vector<addresses::URI> server_uris_;

    //! Connection.
    std::shared_ptr<transport::IConnection> connection_{};

    //! Mutex of connection_ and is_connecting_.
    std::mutex connection_mutex_{};

    //! Whether this connector is stopped.
    std::atomic<bool> is_stopped_{false};

    //! Timer to sleep until next retry.
    ReconnectionTimer retry_timer_;

    //! Callback function called when a connection is established.
    ConnectionCallback on_connection_{};

    //! Callback function called when a message is received.
    MessageReceivedCallback on_received_{};

    //! Callback function called when a message is sent.
    MessageSentCallback on_sent_{};

    //! Callback function called when a connection is closed.
    ConnectionClosedCallback on_closed_{};

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::clients::impl
