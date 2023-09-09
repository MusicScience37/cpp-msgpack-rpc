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
 * \brief Test of parse_toml functions.
 */
#include "msgpack_rpc/config/toml/parse_toml_logging.h"

#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <toml++/toml.h>

#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/logging/log_level.h"

TEST_CASE("msgpack_rpc::config::toml::impl::parse_toml(LoggingConfig)") {
    using msgpack_rpc::config::toml::impl::parse_toml;
    using msgpack_rpc::logging::LogLevel;

    msgpack_rpc::config::LoggingConfig config;

    SECTION("parse an empty table") {
        const auto root_table = toml::parse(R"(
[test]
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));
    }

    SECTION("parse filepath") {
        const auto root_table = toml::parse(R"(
[test]
filepath = "test.log"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.filepath() == "test.log");
    }

    SECTION("parse max_file_size") {
        const auto root_table = toml::parse(R"(
[test]
max_file_size = 12345
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.max_file_size() == 12345);
    }

    SECTION("parse max_files") {
        const auto root_table = toml::parse(R"(
[test]
max_files = 123
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.max_files() == 123);
    }

    SECTION("parse output_log_level") {
        const auto root_table = toml::parse(R"(
[test]
output_log_level = "debug"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.output_log_level() == LogLevel::DEBUG);
    }
}
