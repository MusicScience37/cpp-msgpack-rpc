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
 * \brief Test of CallFutureImpl class.
 */
#include "msgpack_rpc/clients/impl/call_future_impl.h"

#include <chrono>
#include <memory>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/messages/call_result.h"

TEST_CASE("msgpack_rpc::clients::impl::CallFutureImpl") {
    using msgpack_rpc::clients::impl::CallFutureImpl;
    using msgpack_rpc::clients::impl::ICallFutureImpl;
    using msgpack_rpc::messages::CallResult;

    const auto internal_future = std::make_shared<CallFutureImpl>();
    const std::shared_ptr<ICallFutureImpl> user_future = internal_future;

    SECTION("set a result") {
        const auto result_zone = std::make_shared<msgpack::zone>();
        const auto result_object = msgpack::object("abc", *result_zone);
        const auto result =
            CallResult::create_result(result_object, result_zone);

        REQUIRE_NOTHROW(internal_future->set(result));

        SECTION("get the result") {
            const CallResult received_result = user_future->get();

            CHECK(received_result.result_as<std::string_view>() == "abc");
        }

        SECTION("wait the result") {
            const auto timeout = std::chrono::seconds(1);
            const CallResult received_result = user_future->get_within(timeout);

            CHECK(received_result.result_as<std::string_view>() == "abc");
        }
    }

    SECTION("wait the result without a result") {
        const auto timeout = std::chrono::milliseconds(1);
        REQUIRE_THROWS((void)user_future->get_within(timeout));
    }
}
