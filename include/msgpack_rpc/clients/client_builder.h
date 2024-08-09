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
 * \brief Definition of ClientBuilder class.
 */
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/impl/i_client_builder_impl.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc::clients {

/*!
 * \brief Class of builders of clients.
 */
class ClientBuilder {
public:
    /*!
     * \brief Constructor.
     *
     * This overload will use the default configurations for clients and
     * loggers.
     */
    ClientBuilder() : ClientBuilder(logging::Logger::create()) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     *
     * This overload will use the default configurations for clients.
     */
    explicit ClientBuilder(const std::shared_ptr<logging::Logger>& logger)
        : ClientBuilder(config::ClientConfig(), logger) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] client_config Configuration of the client.
     * \param[in] logger Logger.
     */
    explicit ClientBuilder(config::ClientConfig client_config,
        const std::shared_ptr<logging::Logger>& logger =
            logging::Logger::create())
        : impl_(impl::create_default_client_builder_impl(
              std::move(client_config), logger)) {}

    /*!
     * \brief Add a URI to connect to.
     *
     * \param[in] uri URI.
     * \return This.
     */
    ClientBuilder& connect_to(addresses::URI uri) {
        impl_->connect_to(std::move(uri));
        return *this;
    }

    /*!
     * \brief Add a URI to connect to.
     *
     * \param[in] uri URI.
     * \return This.
     */
    ClientBuilder& connect_to(std::string_view uri) {
        return connect_to(addresses::URI::parse(uri));
    }

    /*!
     * \brief Add a TCP address to connect to.
     *
     * \param[in] host Host name.
     * \param[in] port_number Port number.
     * \return This.
     */
    ClientBuilder& connect_to_tcp(
        std::string_view host, std::uint16_t port_number) {
        return connect_to(
            addresses::URI(addresses::TCP_SCHEME, host, port_number));
    }

    /*!
     * \brief Build a client.
     *
     * \return Client.
     */
    [[nodiscard]] Client build() { return Client(impl_->build()); }

private:
    //! Object of the internal implementation.
    std::unique_ptr<impl::IClientBuilderImpl> impl_;
};

}  // namespace msgpack_rpc::clients
