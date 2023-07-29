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
 * \brief Definition of ServerConfig class.
 */
#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::config {

/*!
 * \brief Class of configurations of servers.
 */
class MSGPACK_RPC_EXPORT ServerConfig {
public:
    /*!
     * \brief Constructor.
     *
     * \note No endpoint is registered in constructor, endpoints can be
     * registered via member functions in this class or ServerBuilder class.
     */
    ServerConfig();

    /*!
     * \brief Add a URI of the server.
     *
     * \param[in] uri URI.
     * \return This.
     */
    ServerConfig& add_uri(const addresses::URI& uri);

    /*!
     * \brief Add a URI of the server.
     *
     * \param[in] uri URI.
     * \return This.
     */
    ServerConfig& add_uri(std::string_view uri);

    /*!
     * \brief Get the URIs of the server.
     *
     * \return URIs.
     */
    [[nodiscard]] const std::vector<addresses::URI>& uris() const noexcept;

    /*!
     * \brief Get the configuration of parsers of messages.
     *
     * \return Configuration.
     */
    [[nodiscard]] MessageParserConfig& message_parser() noexcept;

    /*!
     * \brief Get the configuration of parsers of messages.
     *
     * \return Configuration.
     */
    [[nodiscard]] const MessageParserConfig& message_parser() const noexcept;

    /*!
     * \brief Get the configuration of executors.
     *
     * \return Configuration of executors.
     */
    [[nodiscard]] ExecutorConfig& executor() noexcept;

    /*!
     * \brief Get the configuration of executors.
     *
     * \return Configuration of executors.
     */
    [[nodiscard]] const ExecutorConfig& executor() const noexcept;

private:
    //! URIs.
    std::vector<addresses::URI> uris_;

    //! Configuration of parsers of messages.
    MessageParserConfig message_parser_;

    //! Configuration of executors.
    ExecutorConfig executor_;
};

}  // namespace msgpack_rpc::config
