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
 * \brief Definition of IPAcceptorFactory class.
 */
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <asio/error_code.hpp>
#include <asio/ip/basic_resolver_entry.hpp>
#include <asio/ip/basic_resolver_iterator.hpp>
#include <asio/ip/tcp.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/acceptor.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_acceptor_factory.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of factory to create acceptors for protocols based on IP.
 */
class IPAcceptorFactory final : public IAcceptorFactory {
public:
    // TODO Change to template when another protocol is implemented.

    //! Type of resolvers in asio library.
    using AsioResolver = asio::ip::tcp::resolver;

    //! Type of acceptors.
    using AcceptorType = Acceptor;

    //! Type of concrete addresses.
    using ConcreteAddress = addresses::TCPAddress;

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] logger Logger.
     */
    IPAcceptorFactory(std::shared_ptr<executors::IExecutor> executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger)
        : executor_(std::move(executor)),
          message_parser_config_(message_parser_config),
          resolver_(executor_->context(executors::OperationType::TRANSPORT)),
          scheme_("tcp"),
          log_name_(fmt::format("AcceptorFactory({})", scheme_)),
          logger_(std::move(logger)) {}

    //! \copydoc msgpack_rpc::transport::IAcceptorFactory::create
    std::vector<std::shared_ptr<IAcceptor>> create(
        const addresses::URI& uri) override {
        const auto resolved_endpoints = resolve(uri);

        std::vector<std::shared_ptr<IAcceptor>> acceptors;
        acceptors.reserve(resolved_endpoints.size());
        for (const auto& entry : resolved_endpoints) {
            const auto local_address = ConcreteAddress(entry.endpoint());
            std::shared_ptr<IAcceptor> acceptor =
                std::make_shared<AcceptorType>(
                    local_address, executor_, message_parser_config_, logger_);
            acceptors.push_back(std::move(acceptor));
        }

        return acceptors;
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

    //! Executor.
    std::shared_ptr<executors::IExecutor> executor_;

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

}  // namespace msgpack_rpc::transport
