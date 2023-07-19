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
 * \brief Definition of IProtocol class.
 */
#pragma once

#include <memory>
#include <string_view>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_connector.h"
#include "msgpack_rpc/transport/i_resolver.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Interface of protocols.
 *
 * \note Objects of this class are factories of acceptors, connectors, and
 * resolvers.
 */
class IProtocol {
public:
    /*!
     * \brief Get the scheme.
     *
     * \return Scheme.
     */
    [[nodiscard]] virtual std::string_view scheme() const noexcept = 0;

    /*!
     * \brief Create an acceptor to accept connections.
     *
     * \param[in] local_address Local address to listen.
     * \return Acceptor.
     */
    [[nodiscard]] virtual std::shared_ptr<IAcceptor> create_acceptor(
        const addresses::Address& local_address) = 0;

    /*!
     * \brief Create a connector to connect to endpoints of acceptors.
     *
     * \return Connector.
     */
    [[nodiscard]] virtual std::shared_ptr<IConnector> create_connector() = 0;

    /*!
     * \brief Create a resolver.
     *
     * \return Resolver.
     */
    [[nodiscard]] virtual std::shared_ptr<IResolver> create_resolver() = 0;

    IProtocol(const IProtocol&) = delete;
    IProtocol(IProtocol&&) = delete;
    IProtocol& operator=(const IProtocol&) = delete;
    IProtocol& operator=(IProtocol&&) = delete;

    //! Destructor.
    virtual ~IProtocol() noexcept = default;

protected:
    //! Constructor.
    IProtocol() noexcept = default;
};

}  // namespace msgpack_rpc::transport
