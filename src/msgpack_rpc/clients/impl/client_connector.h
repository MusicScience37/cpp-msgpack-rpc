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
#include <cassert>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/async_invoke.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/executors/timer.h"
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

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] backends Backends.
     * \param[in] server_uris URIs of servers.
     * \param[in] logger Logger.
     */
    ClientConnector(const std::shared_ptr<executors::IExecutor>& executor,
        transport::BackendList backends,
        std::vector<addresses::URI> server_uris,
        std::shared_ptr<logging::Logger> logger)
        : executor_(executor),
          backends_(std::move(backends)),
          server_uris_(std::move(server_uris)),
          retry_timer_(executor, executors::OperationType::TRANSPORT),
          logger_(std::move(logger)) {}

    /*!
     * \brief Start processing.
     *
     * \param[in] on_connection Callback function called when a connection is
     * established.
     * \param[in] on_received Callback function called when a message is
     * received.
     * \param[in] on_sent Callback function called when a message is sent.
     */
    void start(ConnectionCallback on_connection,
        MessageReceivedCallback on_received, MessageSentCallback on_sent) {
        on_connection_ = std::move(on_connection);
        on_received_ = std::move(on_received);
        on_sent_ = std::move(on_sent);
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
        std::unique_lock<std::mutex> lock(connection_mutex_);
        connection_ = connection;
        lock.unlock();

        connection->start(on_received_, on_sent_,
            [self = shared_from_this()](
                const Status& /*status*/) { self->on_connection_closed(); });
    }

    /*!
     * \brief Handle failure to connect.
     */
    void on_connection_failure() {
        if (is_stopped_.load(std::memory_order_relaxed)) {
            return;
        }
        // TODO configuration of cycle to retry.
        MSGPACK_RPC_TRACE(
            logger_, "Failed to connect to all URIs, so retry after 100 ms.");
        retry_timer_.async_sleep_for(std::chrono::milliseconds(100),  // NOLINT
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
        async_connect();
    }

    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     */
    [[nodiscard]] std::shared_ptr<executors::IExecutor> executor() {
        auto res = executor_.lock();
        // Executor exists here unless a bug exists.
        assert(res);
        return res;
    }

    //! Executor.
    std::weak_ptr<executors::IExecutor> executor_;

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
    executors::Timer retry_timer_;

    //! Callback function called when a connection is established.
    ConnectionCallback on_connection_{};

    //! Callback function called when a message is received.
    MessageReceivedCallback on_received_{};

    //! Callback function called when a message is sent.
    MessageSentCallback on_sent_{};

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::clients::impl
