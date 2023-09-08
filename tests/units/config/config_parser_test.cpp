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
 * \brief Test of ConfigParser class.
 */
#include "msgpack_rpc/config/config_parser.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::config::ConfigParser") {
    using msgpack_rpc::config::ConfigParser;

    ConfigParser parser;

    SECTION("parse a file") {
        CHECK_NOTHROW(parser.parse("config_parser_test_samples/config1.toml"));

        CHECK(parser.logging_config("example1").filepath() == "test1.log");
    }

    SECTION("parse two files") {
        CHECK_NOTHROW(parser.parse("config_parser_test_samples/config1.toml"));
        CHECK_NOTHROW(parser.parse("config_parser_test_samples/config2.toml"));

        CHECK(parser.logging_config("example1").filepath() == "test1.log");
        CHECK(parser.logging_config("example2").filepath() == "test2.log");
        CHECK(parser.logging_config("example3").filepath() == "test3.log");
    }

    SECTION("try to get non-existing configuration") {
        CHECK_THROWS((void)parser.logging_config("example1"));
    }
}
