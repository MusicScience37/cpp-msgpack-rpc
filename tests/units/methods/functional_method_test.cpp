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

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include "../create_test_logger.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/i_method.h"
#include "msgpack_rpc/methods/method_exception.h"
#include "msgpack_rpc_test/create_parsed_messages.h"
#include "msgpack_rpc_test/parse_messages.h"

TEST_CASE("msgpack_rpc::methods::FunctionalMethod") {
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MethodName;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc::methods::create_functional_method;
    using msgpack_rpc::methods::IMethod;
    using msgpack_rpc::methods::MethodException;
    using msgpack_rpc_test::create_parsed_notification;
    using msgpack_rpc_test::create_parsed_request;
    using msgpack_rpc_test::parse_response;

    const auto logger = msgpack_rpc_test::create_test_logger();

    SECTION("with return values") {
        const auto method_name = MethodName("test_method");
        std::string received_request_param1;
        const std::unique_ptr<IMethod> method =
            create_functional_method<std::string(std::string)>(
                method_name,
                [&received_request_param1](std::string_view str) {
                    received_request_param1 = str;
                    return std::string(str);
                },
                logger);

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
            const auto param1 = std::string_view("parameter");
            const auto notification =
                create_parsed_notification(method_name, param1);

            CHECK_NOTHROW(method->notify(notification));

            CHECK(received_request_param1 == param1);
        }
    }

    SECTION("with a return type but with exceptions in std::runtime_error") {
        const auto method_name = MethodName("test_method");
        std::string received_request_param1;
        const std::unique_ptr<IMethod> method =
            create_functional_method<std::string(std::string)>(
                method_name,
                [&received_request_param1](
                    std::string_view str) -> std::string {
                    received_request_param1 = str;
                    throw std::runtime_error("Test message.");
                },
                logger);

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
            CHECK(parsed_response.result().error_as<std::string_view>() ==
                "Test message.");
        }

        SECTION("notify") {
            const auto param1 = std::string_view("parameter");
            const auto notification =
                create_parsed_notification(method_name, param1);

            CHECK_NOTHROW(method->notify(notification));

            CHECK(received_request_param1 == param1);
        }
    }

    SECTION("with a return type but with exceptions in MethodException") {
        const auto method_name = MethodName("test_method");
        std::string received_request_param1;
        const auto error = std::make_tuple(std::string("Test message."), 12345);
        const std::unique_ptr<IMethod> method =
            create_functional_method<std::string(std::string)>(
                method_name,
                [&received_request_param1, &error](
                    std::string_view str) -> std::string {
                    received_request_param1 = str;
                    throw MethodException(error);  // NOLINT
                },
                logger);

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
            CHECK(parsed_response.result()
                      .error_as<std::tuple<std::string, int>>() == error);
        }

        SECTION("notify") {
            const auto param1 = std::string_view("parameter");
            const auto notification =
                create_parsed_notification(method_name, param1);

            CHECK_NOTHROW(method->notify(notification));

            CHECK(received_request_param1 == param1);
        }
    }

    SECTION("without return values") {
        const auto method_name = MethodName("test_method");
        std::string received_request_param1;
        const std::unique_ptr<IMethod> method =
            create_functional_method<void(std::string)>(
                method_name,
                [&received_request_param1](
                    std::string_view str) { received_request_param1 = str; },
                logger);

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
            CHECK(parsed_response.result().is_success());
        }

        SECTION("notify") {
            const auto param1 = std::string_view("parameter");
            const auto notification =
                create_parsed_notification(method_name, param1);

            CHECK_NOTHROW(method->notify(notification));

            CHECK(received_request_param1 == param1);
        }
    }

    SECTION("without a return type and with exceptions in std::runtime_error") {
        const auto method_name = MethodName("test_method");
        std::string received_request_param1;
        const std::unique_ptr<IMethod> method =
            create_functional_method<void(std::string)>(
                method_name,
                [&received_request_param1](std::string_view str) -> void {
                    received_request_param1 = str;
                    throw std::runtime_error("Test message.");
                },
                logger);

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
            CHECK(parsed_response.result().error_as<std::string_view>() ==
                "Test message.");
        }

        SECTION("notify") {
            const auto param1 = std::string_view("parameter");
            const auto notification =
                create_parsed_notification(method_name, param1);

            CHECK_NOTHROW(method->notify(notification));

            CHECK(received_request_param1 == param1);
        }
    }

    SECTION("without a return type and with exceptions in MethodException") {
        const auto method_name = MethodName("test_method");
        std::string received_request_param1;
        const auto error = std::make_tuple(std::string("Test message."), 12345);
        const std::unique_ptr<IMethod> method =
            create_functional_method<void(std::string)>(
                method_name,
                [&received_request_param1, &error](
                    std::string_view str) -> void {
                    received_request_param1 = str;
                    throw MethodException(error);  // NOLINT
                },
                logger);

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
            CHECK(parsed_response.result()
                      .error_as<std::tuple<std::string, int>>() == error);
        }

        SECTION("notify") {
            const auto param1 = std::string_view("parameter");
            const auto notification =
                create_parsed_notification(method_name, param1);

            CHECK_NOTHROW(method->notify(notification));

            CHECK(received_request_param1 == param1);
        }
    }
}
