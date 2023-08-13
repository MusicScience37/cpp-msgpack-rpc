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
 * \brief Definition of ClientImpl class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of internal implementation of clients.
 */
class ClientImpl final : public IClientImpl {
public:
    //! \copydoc msgpack_rpc::clients::impl::IClientImpl::async_call
    [[nodiscard]] std::shared_ptr<ICallFutureImpl> async_call(
        messages::MethodNameView method_name, messages::MessageID request_id,
        messages::SerializedMessage serialized_request) override {
        // TODO
        (void)method_name;
        (void)request_id;
        (void)serialized_request;
        return std::make_shared<CallFutureImpl>();
    }

private:
};

}  // namespace msgpack_rpc::clients::impl
