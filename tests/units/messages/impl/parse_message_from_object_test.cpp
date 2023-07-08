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
 * \brief Test of parse_message_from_object function.
 */
#include "msgpack_rpc/messages/impl/parse_message_from_object.h"

#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/parsed_response.h"

TEST_CASE("msgpack_rpc::messages::impl::parse_message_from_object") {
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::ParsedNotification;
    using msgpack_rpc::messages::ParsedRequest;
    using msgpack_rpc::messages::ParsedResponse;
    using msgpack_rpc::messages::impl::parse_message_from_object;

    // TODO test of invalid messages.

    SECTION("parse a request") {
        auto zone = std::make_unique<msgpack::zone>();
        const MessageID message_id = 12345;
        const std::string method_name = "method";
        const auto params = std::make_tuple(123);
        const auto object = msgpack::object(
            std::make_tuple(0, message_id, method_name, params), *zone);
        auto object_handle = msgpack::object_handle(object, std::move(zone));

        const auto message =
            parse_message_from_object(std::move(object_handle));

        REQUIRE(message.index() == 0);
        const auto request = std::get<ParsedRequest>(message);
        CHECK(request.id() == message_id);
        CHECK(request.method_name().name() == method_name);
        CHECK(request.parameters().as<int>() == params);
    }

    SECTION("parse a response") {
        auto zone = std::make_unique<msgpack::zone>();
        const MessageID message_id = 12345;
        const auto result = std::string("abc");
        const auto object = msgpack::object(
            std::make_tuple(1, message_id, msgpack::type::nil_t(), result),
            *zone);
        auto object_handle = msgpack::object_handle(object, std::move(zone));

        const auto message =
            parse_message_from_object(std::move(object_handle));

        REQUIRE(message.index() == 1);
        const auto response = std::get<ParsedResponse>(message);
        CHECK(response.id() == message_id);
        CHECK(response.result().result_as<std::string>() == result);
    }

    SECTION("parse a response with an error") {
        auto zone = std::make_unique<msgpack::zone>();
        const MessageID message_id = 12345;
        const auto error = std::string("abc");
        const int result = 123;
        const auto object = msgpack::object(
            std::make_tuple(1, message_id, error, result), *zone);
        auto object_handle = msgpack::object_handle(object, std::move(zone));

        const auto message =
            parse_message_from_object(std::move(object_handle));

        REQUIRE(message.index() == 1);
        const auto response = std::get<ParsedResponse>(message);
        CHECK(response.id() == message_id);
        CHECK(response.result().error_as<std::string>() == error);
    }

    SECTION("parse a notification") {
        auto zone = std::make_unique<msgpack::zone>();
        const std::string method_name = "method";
        const auto params = std::make_tuple(123);
        const auto object =
            msgpack::object(std::make_tuple(2, method_name, params), *zone);
        auto object_handle = msgpack::object_handle(object, std::move(zone));

        const auto message =
            parse_message_from_object(std::move(object_handle));

        REQUIRE(message.index() == 2);
        const auto notification = std::get<ParsedNotification>(message);
        CHECK(notification.method_name().name() == method_name);
        CHECK(notification.parameters().as<int>() == params);
    }
}
