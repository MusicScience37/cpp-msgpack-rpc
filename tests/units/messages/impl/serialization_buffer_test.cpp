/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of SerializationBuffer class.
 */
#include "msgpack_rpc/messages/impl/serialization_buffer.h"

#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/messages/serialized_message.h"

TEST_CASE("msgpack_rpc::messages::impl::SerializationBuffer") {
    using msgpack_rpc::messages::SerializedMessage;
    using msgpack_rpc::messages::impl::SerializationBuffer;

    SECTION("create a serialized message") {
        SerializationBuffer buffer;

        const std::string data1{"abc"};
        buffer.write(data1.data(), data1.size());

        const std::string data2{"de"};
        buffer.write(data2.data(), data2.size());

        const SerializedMessage message = buffer.release();
        CHECK(std::string_view(message.data(), message.size()) == "abcde");
    }

    SECTION("create a large data") {
        SerializationBuffer buffer;

        const std::string data1(10, 'a');
        buffer.write(data1.data(), data1.size());

        const std::string data2(300, 'a');
        buffer.write(data2.data(), data2.size());

        const SerializedMessage message = buffer.release();
        const std::string expected(310, 'a');
        CHECK(std::string_view(message.data(), message.size()) == expected);
    }
}
