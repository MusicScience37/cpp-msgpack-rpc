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
 * \brief Definition of MockClientImpl class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/clients/impl/i_client_impl.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockClientImpl final : public msgpack_rpc::clients::impl::IClientImpl {
    MAKE_MOCK0(start, void(), override);
    MAKE_MOCK0(stop, void(), override);
    MAKE_MOCK2(async_call,
        std::shared_ptr<msgpack_rpc::clients::impl::ICallFutureImpl>(
            msgpack_rpc::messages::MethodNameView,
            const msgpack_rpc::clients::impl::IParametersSerializer&),
        override);
};

}  // namespace msgpack_rpc_test
