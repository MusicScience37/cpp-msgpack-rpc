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
 * \brief Definition of ConfigParser class.
 */
#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::config {

/*!
 * \brief Class to parse configuration.
 */
class MSGPACK_RPC_EXPORT ConfigParser {
public:
    /*!
     * \brief Constructor.
     */
    ConfigParser();

    /*!
     * \brief Parse a file.
     *
     * \param[in] filepath Filepath.
     */
    void parse(std::string_view filepath);

    /*!
     * \brief Get a configuration of logging.
     *
     * \param[in] name Name of the configuration.
     * \return Configuration.
     */
    [[nodiscard]] const LoggingConfig& logging_config(
        std::string_view name) const;

private:
    //! Configurations of logging.
    std::unordered_map<std::string, LoggingConfig> logging_configs_;

    //! Configurations of clients.
    std::unordered_map<std::string, ClientConfig> client_configs_;

    //! Configurations of servers.
    std::unordered_map<std::string, ServerConfig> server_configs_;
};

}  // namespace msgpack_rpc::config
