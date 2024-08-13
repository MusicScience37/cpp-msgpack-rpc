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
 * \brief Definition of TCPConnector class.
 */
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <asio/connect.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/basic_resolver_entry.hpp>
#include <asio/ip/basic_resolver_iterator.hpp>
#include <asio/ip/tcp.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/connection.h"
#include "msgpack_rpc/transport/i_connector.h"

namespace msgpack_rpc::transport::tcp {

/*!
 * \brief Class of connectors fo TCP.
 */
class TCPConnector : public IConnector,
                     public std::enable_shared_from_this<TCPConnector> {
public:
    //! Type of resolvers in asio library.
    using AsioResolver = asio::ip::tcp::resolver;

    //! Type of sockets in asio library.
    using AsioSocket = asio::ip::tcp::socket;

    //! Type of addresses in asio library.
    using AsioAddress = asio::ip::tcp::endpoint;

    //! Type of concrete addresses.
    using ConcreteAddress = addresses::TCPAddress;

    //! Type of connections.
    using ConnectionType = Connection<AsioSocket, ConcreteAddress>;

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] logger Logger.
     */
    TCPConnector(const std::shared_ptr<executors::IExecutor>& executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger)
        : executor_(executor),
          message_parser_config_(message_parser_config),
          resolver_(executor->context(executors::OperationType::TRANSPORT)),
          scheme_("tcp"),
          log_name_(fmt::format("Connector({})", scheme_)),
          logger_(std::move(logger)) {}

    //! \copydoc msgpack_rpc::transport::IConnector::async_connect
    void async_connect(
        const addresses::URI& uri, ConnectionCallback on_connected) override {
        const auto resolved_endpoints = resolve(uri);

        auto socket_ptr = std::make_unique<AsioSocket>(
            get_executor()->context(executors::OperationType::TRANSPORT));
        AsioSocket& socket = *socket_ptr;
        asio::async_connect(socket, resolved_endpoints,
            [self = this->shared_from_this(),
                socket_ptr_moved = std::move(socket_ptr),
                on_connected_moved = std::move(on_connected),
                uri](const asio::error_code& error,
                const AsioAddress& asio_address) {
                self->on_connect(error, std::move(*socket_ptr_moved),
                    on_connected_moved, asio_address, uri);
            });
        MSGPACK_RPC_TRACE(logger_, "({}) Connecting to {}.", log_name_, uri);
    }

private:
    /*!
     * \brief Resolve a URI.
     *
     * \param[in] uri URI.
     * \return List of resolved addresses in asio library.
     */
    [[nodiscard]] AsioResolver::results_type resolve(
        const addresses::URI& uri) {
        if (uri.scheme() != scheme_) {
            throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
                fmt::format("Scheme is different with the resolver: "
                            "expected={}, actual={}",
                    scheme_, uri.scheme()));
        }

        MSGPACK_RPC_TRACE(logger_, "({}) Resolve {}.", log_name_, uri);

        const std::string service = fmt::format(
            "{}", uri.port_number().value_or(static_cast<std::uint16_t>(0)));
        asio::error_code error;
        auto results =
            resolver_.resolve(uri.host_or_filepath(), service, error);
        if (error) {
            const auto message =
                fmt::format("Failed to resolve {}: {}", uri, error.message());
            MSGPACK_RPC_ERROR(logger_, "({}) {}", log_name_, message);
            throw MsgpackRPCException(StatusCode::HOST_UNRESOLVED, message);
        }

        if (logger_->output_log_level() <= logging::LogLevel::TRACE) {
            for (const auto& result : results) {
                MSGPACK_RPC_TRACE(logger_, "({}) Result of resolving {}: {}.",
                    log_name_, uri, fmt::streamed(result.endpoint()));
            }
        }

        return results;
    }

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

    //! Resolver.
    AsioResolver resolver_;

    //! Scheme.
    std::string scheme_;

    //! Name of the connection for logs.
    std::string log_name_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::transport::tcp
