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
 * \brief Definition of IResolver class.
 */
#pragma once

#include <functional>
#include <vector>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/status.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Interface of resolvers.
 */
class IResolver {
public:
    /*!
     * \brief Type of callback functions called when a URI is resolved.
     *
     * Parameters:
     *
     * 1. Status.
     * 2. List of resolved addresses. (Empty when an error occurred.)
     */
    using ResolvedCallback =
        std::function<void(Status, std::vector<addresses::Address>)>;

    /*!
     * \brief Resolve a URI.
     *
     * \param[in] uri URI.
     * \return List of resolved addresses.
     */
    [[nodiscard]] virtual std::vector<addresses::Address> resolve(
        const addresses::URI& uri) = 0;

    IResolver(const IResolver&) = delete;
    IResolver(IResolver&&) = delete;
    IResolver& operator=(const IResolver&) = delete;
    IResolver& operator=(IResolver&&) = delete;

    //! Destructor.
    virtual ~IResolver() noexcept = default;

protected:
    //! Constructor.
    IResolver() noexcept = default;
};

}  // namespace msgpack_rpc::transport
