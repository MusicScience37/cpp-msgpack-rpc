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
#include "msgpack_rpc/config/toml/parse_toml_client_server.h"

#include <chrono>
#include <ratio>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <toml++/toml.h>

#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/config/reconnection_config.h"

TEST_CASE("msgpack_rpc::config::toml::impl::parse_toml(MessageParserConfig)") {
    using msgpack_rpc::config::toml::impl::parse_toml;

    msgpack_rpc::config::MessageParserConfig config;

    SECTION("parse an empty table") {
        const auto root_table = toml::parse(R"(
[test]
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));
    }

    SECTION("parse read_buffer_size") {
        const auto root_table = toml::parse(R"(
[test]
read_buffer_size = 12345
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.read_buffer_size() == 12345);
    }

    SECTION("parse read_buffer_size with invalid value") {
        const auto root_table = toml::parse(R"(
[test]
read_buffer_size = 0
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("read_buffer_size"));
    }

    SECTION("parse read_buffer_size with invalid type") {
        const auto root_table = toml::parse(R"(
[test]
read_buffer_size = "abc"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("read_buffer_size"));
    }
}

TEST_CASE("msgpack_rpc::config::toml::impl::parse_toml(ExecutorConfig)") {
    using msgpack_rpc::config::toml::impl::parse_toml;

    msgpack_rpc::config::ExecutorConfig config;

    SECTION("parse an empty table") {
        const auto root_table = toml::parse(R"(
[test]
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));
    }

    SECTION("parse num_transport_threads") {
        const auto root_table = toml::parse(R"(
[test]
num_transport_threads = 13
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.num_transport_threads() == 13);
    }

    SECTION("parse num_transport_threads with invalid value") {
        const auto root_table = toml::parse(R"(
[test]
num_transport_threads = 0
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("num_transport_threads"));
    }

    SECTION("parse num_transport_threads with invalid type") {
        const auto root_table = toml::parse(R"(
[test]
num_transport_threads = "abc"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("num_transport_threads"));
    }

    SECTION("parse num_callback_threads") {
        const auto root_table = toml::parse(R"(
[test]
num_callback_threads = 17
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.num_callback_threads() == 17);
    }

    SECTION("parse num_callback_threads with invalid value") {
        const auto root_table = toml::parse(R"(
[test]
num_callback_threads = 0
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("num_callback_threads"));
    }

    SECTION("parse num_callback_threads with invalid type") {
        const auto root_table = toml::parse(R"(
[test]
num_callback_threads = "abc"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("num_callback_threads"));
    }
}

TEST_CASE("msgpack_rpc::config::toml::impl::parse_toml(ReconnectionConfig)") {
    using msgpack_rpc::config::toml::impl::parse_toml;

    msgpack_rpc::config::ReconnectionConfig config;

    SECTION("parse an empty table") {
        const auto root_table = toml::parse(R"(
[test]
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));
    }

    SECTION("parse initial_waiting_time_sec") {
        const auto root_table = toml::parse(R"(
[test]
initial_waiting_time_sec = 1.5
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.initial_waiting_time() == std::chrono::milliseconds(1500));
    }

    SECTION("parse initial_waiting_time_sec with invalid value") {
        const auto root_table = toml::parse(R"(
[test]
initial_waiting_time_sec = -1.5
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("initial_waiting_time_sec"));
    }

    SECTION("parse initial_waiting_time_sec with invalid type") {
        const auto root_table = toml::parse(R"(
[test]
initial_waiting_time_sec = "abc"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("initial_waiting_time_sec"));
    }

    SECTION("parse max_waiting_time_sec") {
        const auto root_table = toml::parse(R"(
[test]
max_waiting_time_sec = 1.5
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.max_waiting_time() == std::chrono::milliseconds(1500));
    }

    SECTION("parse max_waiting_time_sec with invalid value") {
        const auto root_table = toml::parse(R"(
[test]
max_waiting_time_sec = -1.5
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("max_waiting_time_sec"));
    }

    SECTION("parse max_waiting_time_sec with invalid type") {
        const auto root_table = toml::parse(R"(
[test]
max_waiting_time_sec = "abc"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("max_waiting_time_sec"));
    }

    SECTION("parse max_jitter_waiting_time_sec") {
        const auto root_table = toml::parse(R"(
[test]
max_jitter_waiting_time_sec = 1.5
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.max_jitter_waiting_time() ==
            std::chrono::milliseconds(1500));
    }

    SECTION("parse max_jitter_waiting_time_sec with invalid value") {
        const auto root_table = toml::parse(R"(
[test]
max_jitter_waiting_time_sec = -1.5
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("max_jitter_waiting_time_sec"));
    }

    SECTION("parse max_jitter_waiting_time_sec with invalid type") {
        const auto root_table = toml::parse(R"(
[test]
max_jitter_waiting_time_sec = "abc"
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        CHECK_THROWS_WITH(parse_toml(test_table, config),
            Catch::Matchers::ContainsSubstring("max_jitter_waiting_time_sec"));
    }
}
