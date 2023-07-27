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
 * \brief Test of ConnectionWrapper class.
 */
#include "msgpack_rpc/transport/connection_wrapper.h"

#include <cstdint>
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "mock_connection.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc_test/parse_messages.h"

TEST_CASE("msgpack_rpc::transport::ConnectionWrapper") {
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MethodNameView;
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc::transport::ConnectionWrapper;
    using msgpack_rpc_test::MockConnection;
    using msgpack_rpc_test::parse_notification;
    using msgpack_rpc_test::parse_request;
    using msgpack_rpc_test::parse_response;
    using trompeloeil::_;

    const auto connection = std::make_shared<MockConnection>();
    ConnectionWrapper wrapper{connection};

    SECTION("copy and move") {
        STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<ConnectionWrapper>);
        STATIC_REQUIRE(std::is_nothrow_copy_assignable_v<ConnectionWrapper>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<ConnectionWrapper>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<ConnectionWrapper>);
    }

    SECTION("send a request") {
        const auto method_name = MethodNameView("test_transport");
        const auto message_id = static_cast<MessageID>(12345);
        const auto param1 = std::string("abc");
        const auto param2 = static_cast<std::int32_t>(456);

        std::shared_ptr<const SerializedMessage> message;
        REQUIRE_CALL(*connection, async_send(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(message = _1);

        wrapper.async_request(method_name, message_id, param1, param2);

        REQUIRE(message);
        const auto request = parse_request(*message);
        CHECK(request.id() == message_id);
        CHECK(request.method_name() == method_name);
        CHECK(request.parameters().as<std::string_view, std::int32_t>() ==
            std::forward_as_tuple(param1, param2));
    }

    SECTION("send a response without an error") {
        const auto message_id = static_cast<MessageID>(12345);
        const auto result = std::make_tuple(123, "abc");

        std::shared_ptr<const SerializedMessage> message;
        REQUIRE_CALL(*connection, async_send(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(message = _1);

        wrapper.async_response_success(message_id, result);

        REQUIRE(message);
        const auto response = parse_response(*message);
        CHECK(response.id() == message_id);
        CHECK(
            response.result().result_as<std::tuple<int, std::string_view>>() ==
            result);
    }

    SECTION("send a response with an error") {
        const auto message_id = static_cast<MessageID>(12345);
        const auto error = std::make_tuple(123, "abc");

        std::shared_ptr<const SerializedMessage> message;
        REQUIRE_CALL(*connection, async_send(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(message = _1);

        wrapper.async_response_error(message_id, error);

        REQUIRE(message);
        const auto response = parse_response(*message);
        CHECK(response.id() == message_id);
        CHECK(response.result().error_as<std::tuple<int, std::string_view>>() ==
            error);
    }

    SECTION("send a notification") {
        const auto method_name = MethodNameView("test_transport");
        const auto param1 = std::string("abc");
        const auto param2 = static_cast<std::int32_t>(456);

        std::shared_ptr<const SerializedMessage> message;
        REQUIRE_CALL(*connection, async_send(_))
            .TIMES(1)
            .LR_SIDE_EFFECT(message = _1);

        wrapper.async_notify(method_name, param1, param2);

        REQUIRE(message);
        const auto notification = parse_notification(*message);
        CHECK(notification.method_name() == method_name);
        CHECK(notification.parameters().as<std::string_view, std::int32_t>() ==
            std::forward_as_tuple(param1, param2));
    }
}
