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
 * \brief Definition of IClientImpl class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/messages/method_name_view.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Interface of internal implementation of clients.
 */
class IClientImpl {
public:
    /*!
     * \brief Stop processing of this client.
     */
    virtual void stop() = 0;

    /*!
     * \brief Asynchronously call a method.
     *
     * \param[in] method_name Name of the method.
     * \param[in] parameters Parameters.
     * \return Future object to wait the result of the call.
     */
    [[nodiscard]] virtual std::shared_ptr<ICallFutureImpl> async_call(
        messages::MethodNameView method_name,
        const IParametersSerializer& parameters) = 0;

    /*!
     * \brief Notify to a method.
     *
     * \param[in] method_name Name of the method.
     * \param[in] parameters Parameters.
     *
     * \note Notifications are always processed asynchronously because a
     * notification doesn't have a response.
     */
    virtual void notify(messages::MethodNameView method_name,
        const IParametersSerializer& parameters) = 0;

    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     *
     * \note This function is mainly for testing. So this function may be
     * removed in the future.
     */
    [[nodiscard]] virtual std::shared_ptr<executors::IExecutor> executor() = 0;

    IClientImpl(const IClientImpl&) = delete;
    IClientImpl(IClientImpl&&) = delete;
    IClientImpl& operator=(const IClientImpl&) = delete;
    IClientImpl& operator=(IClientImpl&&) = delete;

    //! Destructor.
    virtual ~IClientImpl() noexcept = default;

protected:
    //! Constructor.
    IClientImpl() noexcept = default;
};

}  // namespace msgpack_rpc::clients::impl
