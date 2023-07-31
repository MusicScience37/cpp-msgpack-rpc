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
 * \brief Definition of MockBackend class.
 */
#pragma once

#include <memory>
#include <string_view>

#include "msgpack_rpc/addresses/address.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_backend.h"
#include "msgpack_rpc/transport/i_connector.h"
#include "msgpack_rpc/transport/i_resolver.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockBackend final : public msgpack_rpc::transport::IBackend {
public:
    MAKE_CONST_MOCK0(scheme, std::string_view(), noexcept override);

    MAKE_MOCK1(create_acceptor,
        std::shared_ptr<msgpack_rpc::transport::IAcceptor>(
            const msgpack_rpc::addresses::Address&),
        override);

    MAKE_MOCK0(create_connector,
        std::shared_ptr<msgpack_rpc::transport::IConnector>(), override);

    MAKE_MOCK0(create_resolver,
        std::shared_ptr<msgpack_rpc::transport::IResolver>(), override);
};

}  // namespace msgpack_rpc_test
