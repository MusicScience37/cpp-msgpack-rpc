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
 * \brief Test of MessageSerializer class.
 */
#include "msgpack_rpc/messages/message_serializer.h"

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/messages/buffer_view.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/message_parser.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_parameters.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc/messages/serialized_message.h"

TEST_CASE("msgpack_rpc::messages::MessageSerializer") {
    using msgpack_rpc::config::MessageParserConfig;
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MessageParser;
    using msgpack_rpc::messages::MessageSerializer;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::messages::ParsedMessage;
    using msgpack_rpc::messages::ParsedNotification;
    using msgpack_rpc::messages::ParsedRequest;
    using msgpack_rpc::messages::ParsedResponse;
    using msgpack_rpc::messages::SerializedMessage;

    const auto parse_data = [](const SerializedMessage& data) {
        MessageParserConfig config;
        MessageParser parser{config};

        const auto buffer = parser.prepare_buffer();
        REQUIRE(buffer.size() >= data.size());
        std::copy(data.data(), data.data() + data.size(), buffer.data());
        parser.consumed(data.size());

        auto message = parser.try_parse();
        REQUIRE(message.has_value());
        return ParsedMessage(std::move(*message));
    };

    SECTION("serialize a request") {
        const std::string method_name = "method7";
        const MessageID message_id = 12345;
        const int param1 = 123;
        const std::string param2 = "abc";

        const auto data = MessageSerializer::serialize_request(
            MethodNameView(method_name), message_id, param1, param2);

        const auto message = parse_data(data);
        const auto request = std::get<ParsedRequest>(message);
        CHECK(request.id() == message_id);
        CHECK(request.method_name().name() == method_name);
        CHECK(request.parameters().as<int, std::string>() ==
            std::forward_as_tuple(param1, param2));
    }

    SECTION("serialize a request without parameters") {
        const std::string method_name = "method7";
        const MessageID message_id = 12345;

        const auto data = MessageSerializer::serialize_request(
            MethodNameView(method_name), message_id);

        const auto message = parse_data(data);
        const auto request = std::get<ParsedRequest>(message);
        CHECK(request.id() == message_id);
        CHECK(request.method_name().name() == method_name);
        CHECK_NOTHROW((void)request.parameters().as<>());
    }

    SECTION("serialize a response without error") {
        const MessageID message_id = 12345;
        const std::string result = "abc";

        const auto data = MessageSerializer::serialize_successful_response(
            message_id, result);

        const auto message = parse_data(data);
        const auto response = std::get<ParsedResponse>(message);
        CHECK(response.id() == message_id);
        CHECK(response.result().result_as<std::string>() == result);
    }

    SECTION("serialize a response with error") {
        const MessageID message_id = 12345;
        const std::string error = "abc";

        const auto data =
            MessageSerializer::serialize_error_response(message_id, error);

        const auto message = parse_data(data);
        const auto response = std::get<ParsedResponse>(message);
        CHECK(response.id() == message_id);
        CHECK(response.result().error_as<std::string>() == error);
    }

    SECTION("serialize a notification") {
        const std::string method_name = "method7";
        const int param1 = 123;
        const std::string param2 = "abc";

        const auto data = MessageSerializer::serialize_notification(
            MethodNameView(method_name), param1, param2);

        const auto message = parse_data(data);
        const auto notification = std::get<ParsedNotification>(message);
        CHECK(notification.method_name().name() == method_name);
        CHECK(notification.parameters().as<int, std::string>() ==
            std::forward_as_tuple(param1, param2));
    }

    SECTION("serialize a notification without parameters") {
        const std::string method_name = "method7";

        const auto data = MessageSerializer::serialize_notification(
            MethodNameView(method_name));

        const auto message = parse_data(data);
        const auto notification = std::get<ParsedNotification>(message);
        CHECK(notification.method_name().name() == method_name);
        CHECK_NOTHROW((void)notification.parameters().as<>());
    }
}
