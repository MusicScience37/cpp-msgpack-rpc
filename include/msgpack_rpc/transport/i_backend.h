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
 * \brief Definition of IBackend class.
 */
#pragma once

#include <memory>
#include <string_view>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_acceptor_factory.h"
#include "msgpack_rpc/transport/i_connector.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Interface of backends of protocols.
 *
 * \note Objects of this class are factories of acceptor factories, connectors.
 */
class IBackend {
public:
    /*!
     * \brief Get the scheme.
     *
     * \return Scheme.
     */
    [[nodiscard]] virtual std::string_view scheme() const noexcept = 0;

    /*!
     * \brief Create a factory to create acceptors.
     *
     * \return Factory of acceptors.
     */
    [[nodiscard]] virtual std::shared_ptr<IAcceptorFactory>
    create_acceptor_factory() = 0;

    /*!
     * \brief Create a connector to connect to endpoints of acceptors.
     *
     * \return Connector.
     */
    [[nodiscard]] virtual std::shared_ptr<IConnector> create_connector() = 0;

    IBackend(const IBackend&) = delete;
    IBackend(IBackend&&) = delete;
    IBackend& operator=(const IBackend&) = delete;
    IBackend& operator=(IBackend&&) = delete;

    //! Destructor.
    virtual ~IBackend() noexcept = default;

protected:
    //! Constructor.
    IBackend() noexcept = default;
};

}  // namespace msgpack_rpc::transport
