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
 * \brief Definition of TCPProtocol class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_parser.h"
#include "msgpack_rpc/transport/i_protocol.h"

namespace msgpack_rpc::transport::tcp {

/*!
 * \brief Class of TCP.
 */
class TCPProtocol final : public IProtocol {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] logger Logger.
     */
    TCPProtocol(const std::shared_ptr<executors::IExecutor>& executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger);

    //! \copydoc msgpack_rpc::transport::IProtocol::scheme
    [[nodiscard]] std::string_view scheme() const noexcept override;

    //! \copydoc msgpack_rpc::transport::IProtocol::create_acceptor
    [[nodiscard]] std::shared_ptr<IAcceptor> create_acceptor(
        const addresses::Address& local_address) override;

    //! \copydoc msgpack_rpc::transport::IProtocol::create_connector
    [[nodiscard]] std::shared_ptr<IConnector> create_connector() override;

    //! \copydoc msgpack_rpc::transport::IProtocol::create_resolver
    [[nodiscard]] std::shared_ptr<IResolver> create_resolver() override;

    TCPProtocol(const TCPProtocol&) = delete;
    TCPProtocol(TCPProtocol&&) = delete;
    TCPProtocol& operator=(const TCPProtocol&) = delete;
    TCPProtocol& operator=(TCPProtocol&&) = delete;

    //! Destructor.
    ~TCPProtocol() noexcept override;

private:
    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     */
    [[nodiscard]] std::shared_ptr<executors::IExecutor> executor() const;

    //! Executor.
    std::weak_ptr<executors::IExecutor> executor_;

    //! Configuration of parsers of messages.
    config::MessageParserConfig message_parser_config_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::transport::tcp
