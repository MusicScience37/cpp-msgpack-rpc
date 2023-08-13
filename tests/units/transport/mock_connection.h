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
 * \brief Definition of MockConnection class.
 */
#pragma once

#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockConnection final : public msgpack_rpc::transport::IConnection {
public:
    MAKE_MOCK3(start,
        void(MessageReceivedCallback, MessageSentCallback,
            ConnectionClosedCallback),
        override);

    MAKE_MOCK1(async_send,
        void(std::shared_ptr<const msgpack_rpc::messages::SerializedMessage>),
        override);

    MAKE_MOCK0(async_close, void(), override);

    MAKE_CONST_MOCK0(local_address, const msgpack_rpc::addresses::IAddress&(),
        noexcept override);

    MAKE_CONST_MOCK0(remote_address, const msgpack_rpc::addresses::IAddress&(),
        noexcept override);
};

}  // namespace msgpack_rpc_test
