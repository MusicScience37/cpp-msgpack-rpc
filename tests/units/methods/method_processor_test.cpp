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
 * \brief Test of MethodProcessor class.
 */
#include "msgpack_rpc/methods/method_processor.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "../create_test_logger.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/functional_method.h"
#include "msgpack_rpc_test/create_parsed_messages.h"
#include "msgpack_rpc_test/parse_messages.h"

TEST_CASE("msgpack_rpc::methods::MethodProcessor") {
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MethodName;
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc::methods::create_functional_method;
    using msgpack_rpc::methods::create_method_processor;
    using msgpack_rpc_test::create_parsed_notification;
    using msgpack_rpc_test::create_parsed_request;
    using msgpack_rpc_test::parse_response;

    const auto logger = msgpack_rpc_test::create_test_logger();
    const auto processor = create_method_processor(logger);

    SECTION("append a method") {
        std::string received_param1;
        const auto method_name1 = MethodName("test_method1");
        auto method1 = create_functional_method<std::string(std::string)>(
            method_name1,
            [&received_param1](std::string_view str) {
                received_param1 = str;
                return std::string(str);
            },
            logger);

        REQUIRE_NOTHROW(processor->append(std::move(method1)));

        SECTION("and append another method") {
            auto method2 = create_functional_method<std::string(std::string)>(
                "test_method2",
                [](std::string_view str) { return std::string(str); }, logger);

            REQUIRE_NOTHROW(processor->append(std::move(method2)));

            SECTION("and call a method with a request") {
                const auto message_id = static_cast<MessageID>(1234);
                const auto param1 = std::string_view("parameter");
                const auto request =
                    create_parsed_request(method_name1, message_id, param1);

                const SerializedMessage result = processor->call(request);

                CHECK(received_param1 == param1);
                const auto parsed_response = parse_response(result);
                CHECK(parsed_response.id() == message_id);
                CHECK(parsed_response.result().result_as<std::string_view>() ==
                    param1);
            }

            SECTION("and call a method with non-existing method name") {
                const auto method_name = MethodName("non-existing method");
                const auto message_id = static_cast<MessageID>(1234);
                const auto param1 = std::string_view("parameter");
                const auto request =
                    create_parsed_request(method_name, message_id, param1);

                const SerializedMessage result = processor->call(request);

                CHECK(received_param1 == "");  // NOLINT
                const auto parsed_response = parse_response(result);
                CHECK(parsed_response.id() == message_id);
                CHECK(parsed_response.result().is_error());
            }

            SECTION("and notify to a method") {
                const auto param1 = std::string_view("parameter");
                const auto notification =
                    create_parsed_notification(method_name1, param1);

                CHECK_NOTHROW(processor->notify(notification));

                CHECK(received_param1 == param1);
            }

            SECTION("and notify to a method with non-existing name") {
                const auto method_name = MethodName("non-existing method");
                const auto param1 = std::string_view("parameter");
                const auto notification =
                    create_parsed_notification(method_name, param1);

                CHECK_NOTHROW(processor->notify(notification));

                CHECK(received_param1 == "");  // NOLINT
            }
        }

        SECTION("and try to append another method with the same name") {
            auto method2 = create_functional_method<std::string(std::string)>(
                method_name1,
                [](std::string_view str) { return std::string(str); }, logger);

            CHECK_THROWS(processor->append(std::move(method2)));
        }
    }
}
