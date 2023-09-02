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
 * \brief Definition of IAcceptor class.
 */
#pragma once

#include <functional>
#include <memory>

#include "msgpack_rpc/addresses/i_address.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Interface of acceptors to accept connections.
 */
class IAcceptor {
public:
    /*!
     * \brief Type of callback functions called when a connection is accepted.
     *
     * Parameters:
     *
     * 1. Connection.
     */
    using ConnectionCallback =
        std::function<void(std::shared_ptr<IConnection>)>;

    /*!
     * \brief Start process of this acceptor.
     *
     * \param[in] on_connection Callback function called when a connection is
     * accepted.
     */
    virtual void start(ConnectionCallback on_connection) = 0;

    /*!
     * \brief Stop this acceptor.
     */
    virtual void stop() = 0;

    /*!
     * \brief Get the address of the local endpoint.
     *
     * \return Address.
     */
    [[nodiscard]] virtual const addresses::IAddress& local_address()
        const noexcept = 0;

    IAcceptor(const IAcceptor&) = delete;
    IAcceptor(IAcceptor&&) = delete;
    IAcceptor& operator=(const IAcceptor&) = delete;
    IAcceptor& operator=(IAcceptor&&) = delete;

    //! Destructor.
    virtual ~IAcceptor() noexcept = default;

protected:
    //! Constructor.
    IAcceptor() noexcept = default;
};

}  // namespace msgpack_rpc::transport
