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
 * \brief Definition of Client class.
 */
#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "msgpack_rpc/clients/call_future.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/messages/method_name_view.h"

namespace msgpack_rpc::clients {

/*!
 * \brief Class of clients.
 *
 * Clients can be created using ClientBuilder.
 */
class Client {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] impl Object of the internal implementation.
     *
     * \warning Users should create clients using ClientBuilder instead of this
     * constructor.
     */
    explicit Client(std::shared_ptr<impl::IClientImpl> impl)
        : impl_(std::move(impl)) {}

    /*!
     * \brief Stop processing of this client.
     *
     * \note Destructing this client without call of this function will
     * automatically stop this client internally.
     */
    void stop() { impl_->stop(); }

    /*!
     * \brief Asynchronously call a method.
     *
     * \tparam Result Type of the result.
     * \tparam Parameters Types of parameters.
     * \param[in] method_name Name of the method.
     * \param[in] parameters Parameters.
     * \return Future object to get the result of the RPC.
     */
    template <typename Result, typename... Parameters>
    [[nodiscard]] CallFuture<std::decay_t<Result>> async_call(
        messages::MethodNameView method_name, const Parameters&... parameters) {
        return CallFuture<std::decay_t<Result>>{impl_->async_call(
            method_name, impl::make_parameters_serializer(parameters...))};
    }

    /*!
     * \brief Synchronously call a method.
     *
     * \tparam Result Type of the result.
     * \tparam Parameters Types of parameters.
     * \param[in] method_name Name of the method.
     * \param[in] parameters Parameters.
     * \return Result of the RPC.
     *
     * \throw ServerException Errors in the server.
     * \throw MsgpackRPCException Other errors.
     */
    template <typename Result, typename... Parameters>
    std::decay_t<Result> call(
        messages::MethodNameView method_name, const Parameters&... parameters) {
        return async_call<Result>(method_name, parameters...).get_result();
    }

    /*!
     * \brief Notify to a method.
     *
     * \tparam Parameters Types of parameters.
     * \param[in] method_name Name of the method.
     * \param[in] parameters Parameters.
     *
     * \note Notifications are always processed asynchronously because a
     * notification doesn't have a response.
     */
    template <typename... Parameters>
    void notify(
        messages::MethodNameView method_name, const Parameters&... parameters) {
        impl_->notify(
            method_name, impl::make_parameters_serializer(parameters...));
    }

    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     *
     * \note This function is mainly for testing. So this function may be
     * removed in the future.
     */
    [[nodiscard]] std::shared_ptr<executors::IExecutor> executor() {
        return impl_->executor();
    }

private:
    //! Object of the internal implementation.
    std::shared_ptr<impl::IClientImpl> impl_;
};

}  // namespace msgpack_rpc::clients
