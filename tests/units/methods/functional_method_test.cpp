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
 * \brief Test of FunctionalMethod class.
 */
#include "msgpack_rpc/methods/functional_method.h"

#include <memory>
#include <stdexcept>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc_test/create_parsed_messages.h"
#include "msgpack_rpc_test/parse_messages.h"

TEST_CASE("msgpack_rpc::methods::FunctionalMethod") {
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc::methods::create_functional_method;
    using msgpack_rpc_test::create_parsed_request;
    using msgpack_rpc_test::parse_response;

    // TODO interface without types.

    SECTION("with return values") {
        const auto method_name = MethodNameView("test_method");
        std::string received_request_param1;
        const auto method = create_functional_method<std::string(std::string)>(
            method_name, [&received_request_param1](std::string_view str) {
                received_request_param1 = str;
                return std::string(str);
            });

        SECTION("get method name") { CHECK(method->name() == method_name); }

        SECTION("call") {
            const auto message_id = static_cast<MessageID>(1234);
            const auto param1 = std::string_view("parameter");
            const auto request =
                create_parsed_request(method_name, message_id, param1);

            const SerializedMessage result = method->call(request);

            CHECK(received_request_param1 == param1);
            const auto parsed_response = parse_response(result);
            CHECK(parsed_response.id() == message_id);
            CHECK(parsed_response.result().result_as<std::string_view>() ==
                param1);
        }

        SECTION("notify") {
            const auto message_id = static_cast<MessageID>(1234);
            const auto param1 = std::string_view("parameter");
            const auto request =
                create_parsed_request(method_name, message_id, param1);

            CHECK_NOTHROW(method->notify(request));

            CHECK(received_request_param1 == param1);
        }
    }
}
