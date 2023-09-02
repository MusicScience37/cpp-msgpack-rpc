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
 * \brief Definition of MockCallFutureImpl class.
 */
#pragma once

#include <chrono>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/messages/call_result.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockCallFutureImpl final
    : public msgpack_rpc::clients::impl::ICallFutureImpl {
public:
    MAKE_MOCK0(get_result, msgpack_rpc::messages::CallResult(), override);
    MAKE_MOCK1(get_result_within,
        msgpack_rpc::messages::CallResult(std::chrono::nanoseconds), override);
};

}  // namespace msgpack_rpc_test
