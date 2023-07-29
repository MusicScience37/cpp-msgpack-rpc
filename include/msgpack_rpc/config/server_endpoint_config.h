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
 * \brief Definition of ServerEndpointConfig class.
 */
#pragma once

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::config {

/*!
 * \brief Class of configurations of endpoints in servers.
 */
class MSGPACK_RPC_EXPORT ServerEndpointConfig {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] uri URI.
     */
    explicit ServerEndpointConfig(addresses::URI uri);

    /*!
     * \brief Get the URI.
     *
     * \return URI.
     */
    [[nodiscard]] const addresses::URI& uri() const noexcept;

private:
    //! URI.
    addresses::URI uri_;
};

}  // namespace msgpack_rpc::config
