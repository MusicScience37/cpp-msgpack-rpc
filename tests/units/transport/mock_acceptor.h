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
 * \brief Definition of MockAcceptor class.
 */
#pragma once

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockAcceptor final : public msgpack_rpc::transport::IAcceptor {
public:
    MAKE_MOCK1(start, void(ConnectionCallback), override);

    MAKE_MOCK0(stop, void(), override);

    MAKE_CONST_MOCK0(local_address, const msgpack_rpc::addresses::Address&(),
        noexcept override);
};

}  // namespace msgpack_rpc_test
