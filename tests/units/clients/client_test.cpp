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
 * \brief Test of Client class.
 */
#include "msgpack_rpc/clients/client.h"

#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "impl/mock_call_future_impl.h"
#include "impl/mock_client_impl.h"
#include "msgpack_rpc/messages/call_result.h"

TEST_CASE("msgpack_rpc::clients::Client") {
    using msgpack_rpc::clients::Client;
    using msgpack_rpc::messages::CallResult;
    using msgpack_rpc_test::MockCallFutureImpl;
    using msgpack_rpc_test::MockClientImpl;
    using trompeloeil::_;

    const auto client_impl = std::make_shared<MockClientImpl>();

    Client client{client_impl};

    // TODO notifications.
    // TODO exception in methods.

    SECTION("start processing") {
        REQUIRE_CALL(*client_impl, start()).TIMES(1);

        client.start();

        SECTION("and call a method asynchronously") {
            const auto call_future_impl =
                std::make_shared<MockCallFutureImpl>();
            REQUIRE_CALL(*client_impl, async_call(_, _))
                .TIMES(1)
                .RETURN(call_future_impl);

            auto future = client.async_call<std::string>("method1", 3, "abc");

            SECTION("and get the result") {
                const auto result_zone = std::make_shared<msgpack::zone>();
                const auto result_object = msgpack::object("def", *result_zone);
                const auto call_result =
                    CallResult::create_result(result_object, result_zone);
                REQUIRE_CALL(*call_future_impl, get_result())
                    .TIMES(1)
                    .RETURN(call_result);

                const std::string result = future.get_result();
            }
        }

        SECTION("and call a method synchronously") {
            const auto call_future_impl =
                std::make_shared<MockCallFutureImpl>();
            REQUIRE_CALL(*client_impl, async_call(_, _))
                .TIMES(1)
                .RETURN(call_future_impl);

            const auto result_zone = std::make_shared<msgpack::zone>();
            const auto result_object = msgpack::object("def", *result_zone);
            const auto call_result =
                CallResult::create_result(result_object, result_zone);
            REQUIRE_CALL(*call_future_impl, get_result())
                .TIMES(1)
                .RETURN(call_result);

            const std::string result =
                client.call<std::string>("method1", 3, "abc");
        }
    }
}
