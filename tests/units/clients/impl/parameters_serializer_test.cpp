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
 * \brief Test of ParametersSerializer class.
 */
#include "msgpack_rpc/clients/impl/parameters_serializer.h"

#include <string>
#include <string_view>
#include <tuple>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_parameters.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc_test/parse_messages.h"

TEST_CASE("msgpack_rpc::clients::impl::ParametersSerializer") {
    using msgpack_rpc::clients::impl::IParametersSerializer;
    using msgpack_rpc::clients::impl::make_parameters_serializer;
    using msgpack_rpc::clients::impl::ParametersSerializer;
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc_test::parse_notification;
    using msgpack_rpc_test::parse_request;

    SECTION("created without parameters") {
        auto params = make_parameters_serializer();
        const IParametersSerializer& i_params = params;

        SECTION("create serialized request") {
            const auto method_name = MethodNameView("method1");
            const auto request_id = static_cast<MessageID>(12345);

            const SerializedMessage serialized_request =
                i_params.create_serialized_request(method_name, request_id);

            const auto parsed_request = parse_request(serialized_request);
            CHECK(parsed_request.method_name() == method_name);
            CHECK(parsed_request.id() == request_id);
            CHECK_NOTHROW((void)parsed_request.parameters().as<>());
        }

        SECTION("create serialized notification") {
            const auto method_name = MethodNameView("method2");

            const SerializedMessage serialized_notification =
                i_params.create_serialized_notification(method_name);

            const auto parsed_notification =
                parse_notification(serialized_notification);
            CHECK(parsed_notification.method_name() == method_name);
            CHECK_NOTHROW((void)parsed_notification.parameters().as<>());
        }
    }

    SECTION("created with a parameter") {
        const auto param1 = std::string("abc");
        auto params = make_parameters_serializer(param1);
        const IParametersSerializer& i_params = params;

        SECTION("create serialized request") {
            const auto method_name = MethodNameView("method1");
            const auto request_id = static_cast<MessageID>(12345);

            const SerializedMessage serialized_request =
                i_params.create_serialized_request(method_name, request_id);

            const auto parsed_request = parse_request(serialized_request);
            CHECK(parsed_request.method_name() == method_name);
            CHECK(parsed_request.id() == request_id);
            CHECK(parsed_request.parameters().as<std::string_view>() ==
                std::forward_as_tuple(param1));
        }

        SECTION("create serialized notification") {
            const auto method_name = MethodNameView("method2");

            const SerializedMessage serialized_notification =
                i_params.create_serialized_notification(method_name);

            const auto parsed_notification =
                parse_notification(serialized_notification);
            CHECK(parsed_notification.method_name() == method_name);
            CHECK(parsed_notification.parameters().as<std::string_view>() ==
                std::forward_as_tuple(param1));
        }
    }

    SECTION("created with a raw string") {
        auto params = make_parameters_serializer("abc");
        const IParametersSerializer& i_params = params;

        SECTION("create serialized request") {
            const auto method_name = MethodNameView("method1");
            const auto request_id = static_cast<MessageID>(12345);

            const SerializedMessage serialized_request =
                i_params.create_serialized_request(method_name, request_id);

            const auto parsed_request = parse_request(serialized_request);
            CHECK(parsed_request.method_name() == method_name);
            CHECK(parsed_request.id() == request_id);
            CHECK(parsed_request.parameters().as<std::string_view>() ==
                std::forward_as_tuple("abc"));
        }

        SECTION("create serialized notification") {
            const auto method_name = MethodNameView("method2");

            const SerializedMessage serialized_notification =
                i_params.create_serialized_notification(method_name);

            const auto parsed_notification =
                parse_notification(serialized_notification);
            CHECK(parsed_notification.method_name() == method_name);
            CHECK(parsed_notification.parameters().as<std::string_view>() ==
                std::forward_as_tuple("abc"));
        }
    }

    SECTION("created with two parameters") {
        const auto param1 = std::string("abc");
        const int param2 = 123;
        auto params = make_parameters_serializer(param1, param2);
        const IParametersSerializer& i_params = params;

        SECTION("create serialized request") {
            const auto method_name = MethodNameView("method1");
            const auto request_id = static_cast<MessageID>(12345);

            const SerializedMessage serialized_request =
                i_params.create_serialized_request(method_name, request_id);

            const auto parsed_request = parse_request(serialized_request);
            CHECK(parsed_request.method_name() == method_name);
            CHECK(parsed_request.id() == request_id);
            CHECK(parsed_request.parameters().as<std::string_view, int>() ==
                std::forward_as_tuple(param1, param2));
        }

        SECTION("create serialized notification") {
            const auto method_name = MethodNameView("method2");

            const SerializedMessage serialized_notification =
                i_params.create_serialized_notification(method_name);

            const auto parsed_notification =
                parse_notification(serialized_notification);
            CHECK(parsed_notification.method_name() == method_name);
            CHECK(
                parsed_notification.parameters().as<std::string_view, int>() ==
                std::forward_as_tuple(param1, param2));
        }
    }
}
