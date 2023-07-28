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
 * \brief Definition of ServerBuilder class.
 */
#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"

namespace msgpack_rpc::servers {

/*!
 * \brief Class of builders of servers.
 */
class ServerBuilder {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] impl Internal implementation.
     */
    explicit ServerBuilder(std::unique_ptr<impl::IServerBuilderImpl> impl)
        : impl_(std::move(impl)) {}

    /*!
     * \brief Add a URI to listen to.
     *
     * \param[in] uri URI.
     * \return This.
     */
    ServerBuilder& listen_to(addresses::URI uri) {
        impl_->listen_to(std::move(uri));
        return *this;
    }

    /*!
     * \brief Add a TCP address to listen to.
     *
     * \param[in] host Host name.
     * \param[in] port_number Port number.
     * \return This.
     */
    ServerBuilder& listen_to_tcp(
        std::string_view host, std::uint16_t port_number) {
        listen_to(addresses::URI(addresses::TCP_SCHEME, host, port_number));
        return *this;
    }

private:
    //! Internal implementation.
    std::unique_ptr<impl::IServerBuilderImpl> impl_;
};

}  // namespace msgpack_rpc::servers
