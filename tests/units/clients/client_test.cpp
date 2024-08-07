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
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <msgpack.hpp>

#include "impl/mock_call_future_impl.h"
#include "impl/mock_client_impl.h"
#include "msgpack_rpc/clients/server_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/messages/call_result.h"
#include "trompeloeil_catch2.h"

TEST_CASE("msgpack_rpc::clients::Client") {
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::clients::Client;
    using msgpack_rpc::clients::ServerException;
    using msgpack_rpc::messages::CallResult;
    using msgpack_rpc_test::MockCallFutureImpl;
    using msgpack_rpc_test::MockClientImpl;
    using trompeloeil::_;

    const auto client_impl = std::make_shared<MockClientImpl>();

    Client client{client_impl};

    SECTION("start processing") {
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

                CHECK(result == "def");
            }

            SECTION("and get the error") {
                const auto result_zone = std::make_shared<msgpack::zone>();
                const auto result_object =
                    msgpack::object("test message", *result_zone);
                const auto call_result =
                    CallResult::create_error(result_object, result_zone);
                REQUIRE_CALL(*call_future_impl, get_result())
                    .TIMES(1)
                    .RETURN(call_result);

                try {
                    (void)future.get_result();
                    FAIL();
                } catch (const ServerException& e) {
                    CHECK(e.status().code() == StatusCode::SERVER_ERROR);
                    CHECK_THAT(std::string(e.status().message()),
                        Catch::Matchers::ContainsSubstring(
                            "An error in a server: "));
                    CHECK_THAT(std::string(e.status().message()),
                        Catch::Matchers::ContainsSubstring("\"test message\""));
                    CHECK(e.error_as<std::string_view>() == "test message");
                }
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

            CHECK(result == "def");
        }

        SECTION("and call a method without results asynchronously") {
            const auto call_future_impl =
                std::make_shared<MockCallFutureImpl>();
            REQUIRE_CALL(*client_impl, async_call(_, _))
                .TIMES(1)
                .RETURN(call_future_impl);

            auto future = client.async_call<void>("method1", 3, "abc");

            SECTION("and get the result") {
                const auto result_zone = std::make_shared<msgpack::zone>();
                const auto result_object = msgpack::object();
                const auto call_result =
                    CallResult::create_result(result_object, result_zone);
                REQUIRE_CALL(*call_future_impl, get_result())
                    .TIMES(1)
                    .RETURN(call_result);

                CHECK_NOTHROW(future.get_result());
            }

            SECTION("and get the error") {
                const auto result_zone = std::make_shared<msgpack::zone>();
                const auto result_object =
                    msgpack::object("test message", *result_zone);
                const auto call_result =
                    CallResult::create_error(result_object, result_zone);
                REQUIRE_CALL(*call_future_impl, get_result())
                    .TIMES(1)
                    .RETURN(call_result);

                try {
                    future.get_result();
                    FAIL();
                } catch (const ServerException& e) {
                    CHECK(e.status().code() == StatusCode::SERVER_ERROR);
                    CHECK_THAT(std::string(e.status().message()),
                        Catch::Matchers::ContainsSubstring(
                            "An error in a server: "));
                    CHECK_THAT(std::string(e.status().message()),
                        Catch::Matchers::ContainsSubstring("\"test message\""));
                    CHECK(e.error_as<std::string_view>() == "test message");
                }
            }
        }

        SECTION("and call a method without results synchronously") {
            const auto call_future_impl =
                std::make_shared<MockCallFutureImpl>();
            REQUIRE_CALL(*client_impl, async_call(_, _))
                .TIMES(1)
                .RETURN(call_future_impl);

            const auto result_zone = std::make_shared<msgpack::zone>();
            const auto result_object = msgpack::object();
            const auto call_result =
                CallResult::create_result(result_object, result_zone);
            REQUIRE_CALL(*call_future_impl, get_result())
                .TIMES(1)
                .RETURN(call_result);

            client.call<void>("method1", 3, "abc");
        }

        SECTION("and notify to a method") {
            REQUIRE_CALL(*client_impl, notify(_, _)).TIMES(1);

            client.notify("method2", "notification");
        }
    }
}
