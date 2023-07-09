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
 * \brief Test of MessageParserConfig class.
 */
#include "msgpack_rpc/config/message_parser_config.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::config::MessageParserConfig") {
    using msgpack_rpc::config::MessageParserConfig;

    SECTION("set read_buffer_size") {
        MessageParserConfig config;

        constexpr std::size_t value = 1;
        CHECK(config.read_buffer_size(value).read_buffer_size() == value);
    }

    SECTION("set read_buffer_size to wrong value") {
        MessageParserConfig config;

        constexpr std::size_t value = 0;
        CHECK_THROWS(config.read_buffer_size(value));
    }
}
