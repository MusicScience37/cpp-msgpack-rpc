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
 * \brief Definition of MockMethod class.
 */
#pragma once

#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/i_method.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockMethod final : public msgpack_rpc::methods::IMethod {
public:
    MAKE_CONST_MOCK0(
        name, msgpack_rpc::messages::MethodNameView(), noexcept override);

    MAKE_MOCK1(call,
        msgpack_rpc::messages::SerializedMessage(
            const msgpack_rpc::messages::ParsedRequest&),
        override);

    MAKE_MOCK1(notify, void(const msgpack_rpc::messages::ParsedNotification&),
        override);
};

}  // namespace msgpack_rpc_test
