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
 * \brief Definition of MockResolver class.
 */
#pragma once

#include <vector>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/transport/i_resolver.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockResolver final : public msgpack_rpc::transport::IResolver {
public:
    MAKE_MOCK1(resolve,
        std::vector<msgpack_rpc::addresses::Address>(
            const msgpack_rpc::addresses::URI&),
        override);
};

}  // namespace msgpack_rpc_test
