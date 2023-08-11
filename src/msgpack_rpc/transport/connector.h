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
 * \brief Definition of Connector class.
 */
#pragma once

#include <memory>

#include <asio/error_code.hpp>
#include <fmt/core.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/transport/connection.h"
#include "msgpack_rpc/transport/i_connector.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of connectors.
 */
class Connector : public IConnector,
                  public std::enable_shared_from_this<Connector> {
public:
    // TODO Change to template when another protocol is implemented.

    //! Type of sockets in asio library.
    using AsioSocket = asio::ip::tcp::socket;

    //! Type of concrete addresses.
    using ConcreteAddress = addresses::TCPAddress;

    //! Type of connections.
    using ConnectionType = Connection;

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] logger Logger.
     */
    Connector(const std::shared_ptr<executors::IExecutor>& executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger)
        : executor_(executor),
          message_parser_config_(message_parser_config),
          scheme_("tcp"),
          log_name_(fmt::format("Resolver({})", scheme_)),
          logger_(std::move(logger)) {}

    //! \copydoc msgpack_rpc::transport::IConnector::async_connect
    void async_connect(const addresses::Address& address,
        ConnectionCallback on_connected) override {
        auto socket_ptr = std::make_unique<AsioSocket>(
            get_executor()->context(executors::OperationType::TRANSPORT));
        AsioSocket& socket = *socket_ptr;
        socket.async_connect(address.as_tcp().asio_address(),
            [self = this->shared_from_this(),
                socket_ptr_moved = std::move(socket_ptr),
                on_connected_moved = std::move(on_connected),
                address](const asio::error_code& error) {
                self->on_connect(error, std::move(*socket_ptr_moved),
                    on_connected_moved, address);
            });
        MSGPACK_RPC_TRACE(
            logger_, "({}) Connecting to {}.", log_name_, address);
    }

private:
    /*!
     * \brief Handle the result of connect operation.
     *
     * \param[in] error Error.
     * \param[in] socket Socket.
     * \param[in] on_connected Callback function to tell the result to user.
     * \param[in] address Address.
     */
    void on_connect(const asio::error_code& error, AsioSocket&& socket,
        const ConnectionCallback& on_connected,
        const addresses::Address& address) {
        if (error) {
            const auto message = fmt::format(
                "Failed to connect to {}: {}", address, error.message());
            MSGPACK_RPC_WARN(logger_, "({}) {}", log_name_, message);
            on_connected(
                Status(StatusCode::CONNECTION_FAILURE, message), nullptr);
        }

        auto connection = std::make_shared<ConnectionType>(
            std::move(socket), message_parser_config_, logger_);
        on_connected(Status(), std::move(connection));
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

    //! Executor.
    std::weak_ptr<executors::IExecutor> executor_;

    //! Configuration of parsers of messages.
    config::MessageParserConfig message_parser_config_;

    //! Scheme.
    std::string scheme_;

    //! Name of the connection for logs.
    std::string log_name_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::transport
