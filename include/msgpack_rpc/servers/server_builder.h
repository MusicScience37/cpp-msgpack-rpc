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
#include <optional>
#include <string_view>
#include <utility>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/method_name.h"
#include "msgpack_rpc/methods/functional_method.h"
#include "msgpack_rpc/methods/i_method.h"
#include "msgpack_rpc/servers/impl/i_server_builder_impl.h"
#include "msgpack_rpc/servers/server.h"

namespace msgpack_rpc::servers {

/*!
 * \brief Class of builders of servers.
 */
class ServerBuilder {
public:
    /*!
     * \brief Constructor.
     *
     * This overload will use the default configurations for servers and
     * loggers.
     */
    ServerBuilder() : ServerBuilder(logging::Logger::create()) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     *
     * This overload will use the default configurations for servers.
     */
    explicit ServerBuilder(const std::shared_ptr<logging::Logger>& logger)
        : ServerBuilder(config::ServerConfig(), logger) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] server_config Configuration of the server.
     * \param[in] logger Logger.
     */
    explicit ServerBuilder(const config::ServerConfig& server_config,
        const std::shared_ptr<logging::Logger>& logger =
            logging::Logger::create())
        : impl_(impl::create_default_builder_impl(server_config, logger)) {}

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
     * \brief Add a URI to listen to.
     *
     * \param[in] uri String of a URI.
     * \return This.
     */
    ServerBuilder& listen_to(std::string_view uri) {
        impl_->listen_to(addresses::URI::parse(uri));
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

    /*!
     * \brief Add a method.
     *
     * \param[in] method Method.
     * \return This.
     *
     * \note This overload should not be used in most applications.
     */
    ServerBuilder& add_method(std::unique_ptr<methods::IMethod> method) {
        impl_->add_method(std::move(method));
        return *this;
    }

    /*!
     * \brief Add a method implemented by a function object.
     *
     * \tparam Signature Signature of the method.
     * \tparam Function Type of the function implementing the method.
     * \param[in] name Name of the method.
     * \param[in] function Function implementing the method.
     * \return This.
     *
     * \note The function can throw exceptions using
     * msgpack_rpc::methods::MethodException class to notify errors using any
     * serializable objects.
     */
    template <typename Signature, typename Function>
    ServerBuilder& add_method(messages::MethodName name, Function&& function) {
        return add_method(
            methods::create_functional_method<Signature>(std::move(name),
                std::forward<Function>(function), impl_->logger()));
    }

    /*!
     * \brief Build a server.
     *
     * \return Server.
     */
    [[nodiscard]] Server build() { return Server{impl_->build()}; }

private:
    //! Internal implementation.
    std::unique_ptr<impl::IServerBuilderImpl> impl_;
};

}  // namespace msgpack_rpc::servers
