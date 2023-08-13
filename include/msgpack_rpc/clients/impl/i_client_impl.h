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
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Interface of internal implementation of clients.
 */
class IClientImpl {
public:
    /*!
     * \brief Asynchronously call a method.
     *
     * \param[in] method_name Name of the method.
     * \param[in] request_id Message ID of this request.
     * \param[in] serialized_request Serialized request data.
     * \return Future object to wait the result of the call.
     *
     * \note This function assumes that request_id is same with the message ID
     * serialized in serialized_request.
     */
    [[nodiscard]] virtual std::shared_ptr<ICallFutureImpl> async_call(
        messages::MethodNameView method_name, messages::MessageID request_id,
        messages::SerializedMessage serialized_request) = 0;

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
