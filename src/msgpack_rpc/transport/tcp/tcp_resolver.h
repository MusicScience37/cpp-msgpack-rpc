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
 * \brief Definition of TCPResolver class.
 */
#pragma once

#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/i_resolver.h"
#include "msgpack_rpc/transport/ip_resolver_impl.h"

namespace msgpack_rpc::transport::tcp {

/*!
 * \brief Class of resolvers for TCP.
 */
class TCPResolver final : public IResolver {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] context Context in asio library.
     * \param[in] logger Logger.
     */
    TCPResolver(executors::AsioContextType& context,
        std::shared_ptr<logging::Logger> logger);

    //! \copydoc msgpack_rpc::transport::IResolver::resolve
    [[nodiscard]] std::vector<addresses::Address> resolve(
        const addresses::URI& uri) override;

    TCPResolver(const TCPResolver&) = delete;
    TCPResolver(TCPResolver&&) = delete;
    TCPResolver& operator=(const TCPResolver&) = delete;
    TCPResolver& operator=(TCPResolver&&) = delete;

    //! Destructor.
    ~TCPResolver() override;

private:
    //! Internal resolver.
    IPResolverImpl impl_;
};

}  // namespace msgpack_rpc::transport::tcp
