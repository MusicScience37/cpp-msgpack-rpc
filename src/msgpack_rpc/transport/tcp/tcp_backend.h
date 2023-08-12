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
 * \brief Definition of TCPBackend class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_parser.h"
#include "msgpack_rpc/transport/i_backend.h"

namespace msgpack_rpc::transport::tcp {

/*!
 * \brief Class of backend of TCP.
 */
class TCPBackend final : public IBackend {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] logger Logger.
     */
    TCPBackend(const std::shared_ptr<executors::IExecutor>& executor,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger);

    //! \copydoc msgpack_rpc::transport::IBackend::scheme
    [[nodiscard]] std::string_view scheme() const noexcept override;

    //! \copydoc msgpack_rpc::transport::IBackend::create_acceptor_factory
    [[nodiscard]] std::shared_ptr<IAcceptorFactory> create_acceptor_factory()
        override;

    //! \copydoc msgpack_rpc::transport::IBackend::create_connector
    [[nodiscard]] std::shared_ptr<IConnector> create_connector() override;

    TCPBackend(const TCPBackend&) = delete;
    TCPBackend(TCPBackend&&) = delete;
    TCPBackend& operator=(const TCPBackend&) = delete;
    TCPBackend& operator=(TCPBackend&&) = delete;

    //! Destructor.
    ~TCPBackend() noexcept override;

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
