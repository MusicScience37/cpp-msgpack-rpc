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
 * \brief Test of MessageParser class.
 */
#include "msgpack_rpc/messages/message_parser.h"

#include <algorithm>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_parameters.h"
#include "msgpack_rpc/messages/parsed_request.h"

TEST_CASE("msgpack_rpc::messages::MessageParser") {
    using msgpack_rpc::config::MessageParserConfig;
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc::messages::MessageParser;
    using msgpack_rpc::messages::ParsedMessage;
    using msgpack_rpc::messages::ParsedRequest;

    const auto create_data = [](const auto& data) {
        msgpack::sbuffer buffer;
        msgpack::pack(buffer, data);
        return std::string(buffer.data(), buffer.size());
    };

    SECTION("parse a message") {
        const MessageID message_id = 12345;
        const std::string method_name = "method";
        const auto params = std::make_tuple(123);
        const auto data =
            create_data(std::make_tuple(0, message_id, method_name, params));
        const std::size_t data_size = data.size();

        MessageParserConfig config;
        MessageParser parser{config};

        const auto buffer1 = parser.prepare_buffer();
        const std::size_t written_size1 = data_size / 2U;
        REQUIRE(buffer1.size() >= written_size1);
        std::copy(data.data(), data.data() + written_size1, buffer1.data());
        parser.consumed(written_size1);

        std::optional<ParsedMessage> message1;
        REQUIRE_NOTHROW(message1 = parser.try_parse());
        CHECK_FALSE(message1.has_value());

        const auto buffer2 = parser.prepare_buffer();
        REQUIRE(buffer2.size() >= data_size - written_size1);
        std::copy(data.data() + written_size1, data.data() + data_size,
            buffer2.data());
        parser.consumed(data_size - written_size1);

        std::optional<ParsedMessage> message2;
        REQUIRE_NOTHROW(message2 = parser.try_parse());
        REQUIRE(message2.has_value());

        REQUIRE(message2->index() == 0);
        const auto request = std::get<ParsedRequest>(*message2);
        CHECK(request.id() == message_id);
        CHECK(request.method_name().name() == method_name);
        CHECK(request.parameters().as<int>() == params);
    }

    SECTION("parse invalid data") {
        MessageParserConfig config;
        MessageParser parser{config};
        const auto invalid_char =
            static_cast<char>(static_cast<unsigned char>(0xC1));

        const auto buffer = parser.prepare_buffer();
        *(buffer.data()) = invalid_char;
        parser.consumed(1U);
        CHECK_THROWS((void)parser.try_parse());
    }
}
