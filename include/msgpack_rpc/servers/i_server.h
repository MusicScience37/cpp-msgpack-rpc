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
 * \brief Definition of IServer class.
 */
#pragma once

#include <vector>

#include "msgpack_rpc/addresses/address.h"
namespace msgpack_rpc::servers {

/*!
 * \brief Interface of servers.
 */
class IServer {
public:
    /*!
     * \brief Start processing of this server.
     */
    virtual void start() = 0;

    /*!
     * \brief Stop processing of this server.
     */
    virtual void stop() = 0;

    /*!
     * \brief Get the addresses of the local endpoints in this server.
     *
     * \return Addresses.
     */
    [[nodiscard]] virtual std::vector<addresses::Address> local_addresses() = 0;

    IServer(const IServer&) = delete;
    IServer(IServer&&) = delete;
    IServer& operator=(const IServer&) = delete;
    IServer& operator=(IServer&&) = delete;

    //! Destructor.
    virtual ~IServer() noexcept = default;

protected:
    //! Constructor.
    IServer() noexcept = default;
};

}  // namespace msgpack_rpc::servers
