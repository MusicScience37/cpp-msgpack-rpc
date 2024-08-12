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
 * \brief Definition of Acceptor class.
 */
#pragma once

#include <cstdio>  // IWYU pragma: keep
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <type_traits>  // IWYU pragma: keep
#include <utility>

#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/post.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/addresses/i_address.h"
#include "msgpack_rpc/addresses/unix_socket_address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/background_task_state_machine.h"
#include "msgpack_rpc/transport/connection.h"
#include "msgpack_rpc/transport/connection_list.h"
#include "msgpack_rpc/transport/i_acceptor.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of acceptors.
 */

template <typename AsioAcceptorType, typename AsioSocketType,
    typename ConcreteAddressType>
class Acceptor
    : public IAcceptor,
      public std::enable_shared_from_this<
          Acceptor<AsioAcceptorType, AsioSocketType, ConcreteAddressType>> {
public:
    //! Type of acceptors in asio library.
    using AsioAcceptor = AsioAcceptorType;

    //! Type of sockets in asio library.
    using AsioSocket = AsioSocketType;

    //! Type of concrete addresses.
    using ConcreteAddress = ConcreteAddressType;

    //! Type of connections.
    using ConnectionType = Connection<AsioSocket, ConcreteAddress>;

    /*!
     * \brief Constructor.
     *
     * \param[in] local_address Local address.
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of the parser of messages.
     * \param[in] logger Logger.
     */
    Acceptor(const ConcreteAddress& local_address,
        const std::shared_ptr<executors::IExecutor>& executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger)
        : acceptor_(executor->context(executors::OperationType::TRANSPORT),
              local_address.asio_address()),
          executor_(executor),
          local_address_(acceptor_.local_endpoint()),
          message_parser_config_(message_parser_config),
          log_name_(fmt::format("Acceptor(local={})", local_address_)),
          logger_(std::move(logger)),
          connection_list_(std::make_shared<ConnectionList<ConnectionType>>()) {
        MSGPACK_RPC_TRACE(logger_, "({}) Created an acceptor to listen {}.",
            log_name_, local_address_);
    }

    //! \copydoc msgpack_rpc::transport::IAcceptor::start
    void start(ConnectionCallback on_connection) override {
        state_machine_.handle_start_request();
        // Only one thread can enter here.

        on_connection_ = std::move(on_connection);
        asio::post(acceptor_.get_executor(),
            [self = this->shared_from_this()] { self->async_accept_next(); });

        state_machine_.handle_processing_started();
    }

    //! \copydoc msgpack_rpc::transport::IAcceptor::stop
    void stop() override {
        asio::post(acceptor_.get_executor(),
            [self = this->shared_from_this()]() { self->stop_in_thread(); });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::local_address
    [[nodiscard]] const addresses::IAddress& local_address()
        const noexcept override {
        return local_address_;
    }

private:
    /*!
     * \brief Asynchronously accept a connection.
     */
    void async_accept_next() {
        socket_.reset();
        socket_.emplace(
            get_executor()->context(executors::OperationType::TRANSPORT));
        acceptor_.async_accept(*socket_,
            [self = this->shared_from_this()](
                const asio::error_code& error) { self->on_accept(error); });
    }

    /*!
     * \brief Handle the result of accept operation.
     *
     * \param[in] error Error.
     */
    void on_accept(const asio::error_code& error) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            const auto message =
                fmt::format("Error occurred when accepting a connection: {}",
                    error.message());
            MSGPACK_RPC_ERROR(logger_, "({}) {}", log_name_, message);
            throw MsgpackRPCException(StatusCode::UNEXPECTED_ERROR, message);
        }

        MSGPACK_RPC_TRACE(logger_, "({}) Accepted a connection from {}.",
            log_name_, fmt::streamed(socket_->remote_endpoint()));
        auto connection = std::make_shared<ConnectionType>(std::move(*socket_),
            message_parser_config_, logger_, connection_list_);
        connection_list_->append(connection);
        on_connection_(std::move(connection));

        if (!state_machine_.is_processing()) {
            return;
        }
        async_accept_next();
    }

    /*!
     * \brief Stop this acceptor in this thread.
     */
    void stop_in_thread() {
        if (!state_machine_.handle_stop_requested()) {
            return;
        }
        acceptor_.cancel();
        acceptor_.close();
        connection_list_->async_close_all();
        if constexpr (std::is_same_v<ConcreteAddress,
                          addresses::UnixSocketAddress>) {
            // Without removing the file of Unix socket, the socket cannot be
            // used next time.
            (void)std::remove(local_address_.filepath().c_str());
        }
        MSGPACK_RPC_TRACE(logger_, "({}) Stopped this acceptor.", log_name_);
    }

    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     */
    [[nodiscard]] std::shared_ptr<executors::IExecutor> get_executor() {
        auto executor = executor_.lock();
        if (!executor) {
            const auto message = std::string("Executor is not set.");
            MSGPACK_RPC_CRITICAL(logger_, "({}) {}", log_name_, message);
            throw MsgpackRPCException(
                StatusCode::PRECONDITION_NOT_MET, message);
        }
        return executor;
    }

    //! Acceptor.
    AsioAcceptor acceptor_;

    //! Socket to accept connections.
    std::optional<AsioSocket> socket_{};

    //! Executor.
    std::weak_ptr<executors::IExecutor> executor_;

    //! Callback function called when a connection is accepted.
    ConnectionCallback on_connection_{};

    //! Address of the local endpoint.
    ConcreteAddress local_address_;

    //! Configuration of parsers of messages.
    config::MessageParserConfig message_parser_config_;

    //! Name of the connection for logs.
    std::string log_name_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! State machine.
    BackgroundTaskStateMachine state_machine_{};

    //! List of connections.
    std::shared_ptr<ConnectionList<ConnectionType>> connection_list_;
};

}  // namespace msgpack_rpc::transport
