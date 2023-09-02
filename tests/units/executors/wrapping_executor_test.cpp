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
 * \brief Test of WrappingExecutor class.
 */
#include <memory>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mock_executor.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/executors/wrap_executor.h"
#include "trompeloeil_catch2.h"

TEST_CASE("msgpack_rpc::executors::WrappingExecutor") {
    using msgpack_rpc::executors::AsioContextType;
    using msgpack_rpc::executors::OperationType;
    using msgpack_rpc::executors::wrap_executor;
    using msgpack_rpc_test::MockExecutor;
    using trompeloeil::_;

    const auto wrapped = std::make_shared<MockExecutor>();
    const auto wrapper = wrap_executor(wrapped);

    SECTION("get a context") {
        const auto type =
            GENERATE(OperationType::TRANSPORT, OperationType::CALLBACK);
        AsioContextType context;
        REQUIRE_CALL(*wrapped, context(type)).TIMES(1).LR_RETURN(context);

        CHECK_NOTHROW((void)wrapper->context(type));
    }

    SECTION("do nothing in other functions") {
        FORBID_CALL(*wrapped, context(_));

        CHECK_NOTHROW(wrapper->start());
        CHECK_NOTHROW(wrapper->stop());
        CHECK_NOTHROW(wrapper->last_exception());
    }
}
