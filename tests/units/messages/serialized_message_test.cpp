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
 * \brief Test of SerializedMessage class.
 */
#include "msgpack_rpc/messages/serialized_message.h"

#include <cstring>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::messages::SerializedMessage") {
    using msgpack_rpc::messages::SerializedMessage;

    SECTION("create a buffer") {
        const std::vector<char> raw_data(3, 'a');

        const SerializedMessage message{raw_data.data(), raw_data.size()};

        REQUIRE(message.size() == raw_data.size());
        CHECK(
            std::memcmp(message.data(), raw_data.data(), message.size()) == 0);
    }

    SECTION("use copy constructor") {
        const std::vector<char> raw_data(3, 'a');
        const SerializedMessage origin{raw_data.data(), raw_data.size()};

        const SerializedMessage copy{origin};  // NOLINT

        REQUIRE(copy.size() == raw_data.size());
        CHECK(std::memcmp(copy.data(), raw_data.data(), copy.size()) == 0);
    }

    SECTION("use move constructor") {
        const std::vector<char> raw_data(3, 'a');
        SerializedMessage origin{raw_data.data(), raw_data.size()};

        const SerializedMessage moved{std::move(origin)};  // NOLINT

        REQUIRE(moved.size() == raw_data.size());
        CHECK(std::memcmp(moved.data(), raw_data.data(), moved.size()) == 0);
    }

    SECTION("use copy assignment operator") {
        const std::vector<char> raw_data(3, 'a');
        const SerializedMessage origin{raw_data.data(), raw_data.size()};

        SerializedMessage copy{"", 0};
        copy = origin;

        REQUIRE(copy.size() == raw_data.size());
        CHECK(std::memcmp(copy.data(), raw_data.data(), copy.size()) == 0);
    }

    SECTION("use move assignment operator") {
        const std::vector<char> raw_data(3, 'a');
        SerializedMessage origin{raw_data.data(), raw_data.size()};

        SerializedMessage moved{"", 0};
        moved = std::move(origin);  // NOLINT

        REQUIRE(moved.size() == raw_data.size());
        CHECK(std::memcmp(moved.data(), raw_data.data(), moved.size()) == 0);
    }
}
