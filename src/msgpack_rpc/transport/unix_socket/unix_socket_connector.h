/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of UnixSocketConnector class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_UNIX_SOCKETS

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <asio/error_code.hpp>
#include <asio/local/stream_protocol.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/addresses/unix_socket_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/connection.h"
#include "msgpack_rpc/transport/i_connector.h"

namespace msgpack_rpc::transport::unix_socket {

/*!
 * \brief Class of connectors of Unix sockets.
 */
class UnixSocketConnector final
    : public IConnector,
      public std::enable_shared_from_this<UnixSocketConnector> {
public:
    //! Type of sockets in asio library.
    using AsioSocket = asio::local::stream_protocol::socket;

    //! Type of addresses in asio library.
    using AsioAddress = asio::local::stream_protocol::endpoint;

    //! Type of concrete addresses.
    using ConcreteAddress = addresses::UnixSocketAddress;

    //! Type of connections.
    using ConnectionType = Connection<AsioSocket, ConcreteAddress>;

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] logger Logger.
     * \param[in] scheme Scheme.
     */
    UnixSocketConnector(const std::shared_ptr<executors::IExecutor>& executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger, std::string_view scheme)
        : executor_(executor),
          message_parser_config_(message_parser_config),
          scheme_(scheme),
          log_name_(fmt::format("Connector({})", scheme_)),
          logger_(std::move(logger)) {}

    //! \copydoc msgpack_rpc::transport::IConnector::async_connect
    void async_connect(
        const addresses::URI& uri, ConnectionCallback on_connected) override {
        const AsioAddress asio_address{uri.host_or_file_path()};

        auto socket_ptr = std::make_unique<AsioSocket>(
            get_executor()->context(executors::OperationType::TRANSPORT));
        AsioSocket& socket = *socket_ptr;
        socket.async_connect(asio_address,
            [self = this->shared_from_this(),
                socket_ptr_moved = std::move(socket_ptr),
                on_connected_moved = std::move(on_connected), uri,
                asio_address](const asio::error_code& error) {
                self->on_connect(error, std::move(*socket_ptr_moved),
                    on_connected_moved, asio_address, uri);
            });
        MSGPACK_RPC_TRACE(logger_, "({}) Connecting to {}.", log_name_, uri);
    }

private:
    /*!
     * \brief Handle the result of connect operation.
     *
     * \param[in] error Error.
     * \param[in] socket Socket.
     * \param[in] on_connected Callback function to tell the result to user.
     * \param[in] asio_address Address in asio library.
     * \param[in] uri URI.
     */
    void on_connect(const asio::error_code& error, AsioSocket&& socket,
        const ConnectionCallback& on_connected, const AsioAddress& asio_address,
        const addresses::URI& uri) {
        if (error) {
            const auto message = fmt::format(
                "Failed to connect to {}: {}", uri, error.message());
            MSGPACK_RPC_WARN(logger_, "({}) {}", log_name_, message);
            on_connected(
                Status(StatusCode::CONNECTION_FAILURE, message), nullptr);
            return;
        }
        MSGPACK_RPC_TRACE(logger_, "({}) Connected to {}.", log_name_,
            fmt::streamed(asio_address));

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

}  // namespace msgpack_rpc::transport::unix_socket

#endif
