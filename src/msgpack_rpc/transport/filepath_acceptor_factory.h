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
 * \brief Definition of FilepathAcceptorFactory class.
 */
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <asio/error_code.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/addresses/unix_socket_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_acceptor_factory.h"
#include "msgpack_rpc/transport/unix_socket/unix_socket_acceptor.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of factory to create acceptors for protocols using filepaths.
 */
class FilepathAcceptorFactory final : public IAcceptorFactory {
public:
    // TODO Change to template when another protocol is implemented.

    //! Type of acceptors.
    using AcceptorType = unix_socket::UnixSocketAcceptor;

    //! Type of concrete addresses.
    using ConcreteAddress = addresses::UnixSocketAddress;

    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] logger Logger.
     * \param[in] scheme Scheme.
     */
    FilepathAcceptorFactory(std::shared_ptr<executors::IExecutor> executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger, std::string_view scheme)
        : executor_(std::move(executor)),
          message_parser_config_(message_parser_config),
          scheme_(scheme),
          log_name_(fmt::format("AcceptorFactory({})", scheme_)),
          logger_(std::move(logger)) {}

    //! \copydoc msgpack_rpc::transport::IAcceptorFactory::create
    std::vector<std::shared_ptr<IAcceptor>> create(
        const addresses::URI& uri) override {
        const ConcreteAddress local_address(uri.host_or_filepath());
        return std::vector<std::shared_ptr<IAcceptor>>{
            std::make_shared<AcceptorType>(
                local_address, executor_, message_parser_config_, logger_)};
    }

private:
    //! Executor.
    std::shared_ptr<executors::IExecutor> executor_;

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
