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
 * \brief Definition of IPResolver class.
 */
#pragma once

#include <memory>
#include <string>
#include <utility>

#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/system_error.hpp>
#include <fmt/core.h>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/i_resolver.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of resolvers for protocols based on IP.
 */
class IPResolver : public IResolver {
public:
    // TODO Change to template when another protocol is implemented.

    //! Type of resolvers in asio library.
    using AsioResolver = asio::ip::tcp::resolver;

    //! Type of concrete addresses.
    using ConcreteAddressType = addresses::TCPAddress;

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     */
    IPResolver(const std::shared_ptr<executors::IExecutor>& executor,
        std::shared_ptr<logging::Logger> logger)
        : resolver_(executor->context(executors::OperationType::TRANSPORT)),
          scheme_("tcp"),
          log_name_(fmt::format("Resolver({})", scheme_)),
          logger_(std::move(logger)) {}

    /*!
     * \brief Resolve a URI.
     *
     * \param[in] uri URI.
     * \return List of resolved addresses.
     */
    [[nodiscard]] std::vector<addresses::Address> resolve(
        const addresses::URI& uri) override {
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
        const auto results = resolver_.resolve(uri.host(), service, error);
        if (error) {
            const auto message =
                fmt::format("Failed to resolve {}: {}", uri, error.message());
            MSGPACK_RPC_ERROR(logger_, "({}) {}", log_name_, message);
            throw MsgpackRPCException(StatusCode::HOST_UNRESOLVED, message);
        }

        std::vector<addresses::Address> resolved_addresses;
        resolved_addresses.reserve(results.size());
        for (const auto& result : results) {
            auto address = ConcreteAddressType(result.endpoint());
            MSGPACK_RPC_TRACE(logger_, "({}) Result of resolving {}: {}.",
                log_name_, uri, address);
            resolved_addresses.emplace_back(std::move(address));
        }
        return resolved_addresses;
    }

private:
    //! Resolver.
    AsioResolver resolver_;

    //! Scheme.
    std::string scheme_;

    //! Name of the resolver for logs.
    std::string log_name_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::transport
