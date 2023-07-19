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
 * \brief Definition of IConnector class.
 */
#pragma once

#include <functional>
#include <memory>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Interface of connectors to connect to endpoints of acceptors.
 */
class IConnector {
public:
    /*!
     * \brief Type of callback functions called when connecting process finished
     * (even for failures).
     *
     * Parameters:
     *
     * 1. Status.
     * 2. Connection. (Null for failure.)
     */
    using ConnectionCallback =
        std::function<void(const Status&, std::shared_ptr<IConnection>)>;

    /*!
     * \brief Asynchronously connect to an endpoint.
     *
     * \param[in] address Address of the endpoint.
     * \param[in] on_connected Callback function called when the process
     * finished.
     */
    virtual void async_connect(
        const addresses::Address& address, ConnectionCallback on_connected) = 0;

    IConnector(const IConnector&) = delete;
    IConnector(IConnector&&) = delete;
    IConnector& operator=(const IConnector&) = delete;
    IConnector& operator=(IConnector&&) = delete;

    //! Destructor.
    virtual ~IConnector() noexcept = default;

protected:
    //! Constructor.
    IConnector() noexcept = default;
};

}  // namespace msgpack_rpc::transport
