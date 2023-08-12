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
 * \brief Definition of IAcceptorFactory class.
 */
#pragma once

#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/transport/i_acceptor.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Interface of factories to create acceptors.
 */
class IAcceptorFactory {
public:
    /*!
     * \brief Create acceptors for a URI.
     *
     * \param[in] uri URI.
     * \return Acceptors.
     */
    [[nodiscard]] virtual std::vector<std::shared_ptr<IAcceptor>> create(
        const addresses::URI& uri) = 0;

    IAcceptorFactory(const IAcceptorFactory&) = delete;
    IAcceptorFactory(IAcceptorFactory&&) = delete;
    IAcceptorFactory& operator=(const IAcceptorFactory&) = delete;
    IAcceptorFactory& operator=(IAcceptorFactory&&) = delete;

    //! Destructor.
    virtual ~IAcceptorFactory() noexcept = default;

protected:
    //! Constructor.
    IAcceptorFactory() noexcept = default;
};

}  // namespace msgpack_rpc::transport
