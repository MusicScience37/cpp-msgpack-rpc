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
 * \brief Definition of MockExecutor class.
 */
#pragma once

#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "trompeloeil_catch2.h"

namespace msgpack_rpc_test {

class MockExecutor final : public msgpack_rpc::executors::IExecutor {
public:
    MAKE_MOCK1(context,
        msgpack_rpc::executors::AsioContextType&(
            msgpack_rpc::executors::OperationType),
        noexcept override);
};

}  // namespace msgpack_rpc_test
