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
 * \brief Definition of IAddress.
 */
#pragma once

#include "msgpack_rpc/addresses/uri.h"

namespace msgpack_rpc::addresses {

/*!
 * \brief Interface of addresses.
 */
class IAddress {
public:
    /*!
     * \brief Convert to URI.
     *
     * \return URI.
     */
    [[nodiscard]] virtual URI to_uri() const = 0;

#ifndef MSGPACK_RPC_DOCUMENTATION
    IAddress(const IAddress&) = default;
    IAddress(IAddress&&) = default;
    IAddress& operator=(const IAddress&) = default;
    IAddress& operator=(IAddress&&) = default;
#endif

    //! Destructor.
    virtual ~IAddress() noexcept = default;

protected:
    //! Constructor.
    IAddress() noexcept = default;
};

}  // namespace msgpack_rpc::addresses
