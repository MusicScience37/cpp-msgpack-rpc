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
 * \brief Definition of MockConnector class.
 */
#pragma once

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/transport/i_connector.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockConnector final : public msgpack_rpc::transport::IConnector {
public:
    MAKE_MOCK2(async_connect,
        void(const msgpack_rpc::addresses::URI&, ConnectionCallback), override);
};

}  // namespace msgpack_rpc_test
