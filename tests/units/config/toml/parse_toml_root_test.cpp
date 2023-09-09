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
#include "msgpack_rpc/config/toml/parse_toml_root.h"

#include <functional>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/server_config.h"

TEST_CASE("msgpack_rpc::config::toml::impl::parse_toml(root node)") {
    using msgpack_rpc::config::ClientConfig;
    using msgpack_rpc::config::LoggingConfig;
    using msgpack_rpc::config::ServerConfig;
    using msgpack_rpc::config::toml::impl::parse_toml;

    std::unordered_map<std::string, LoggingConfig> logging_configs;
    std::unordered_map<std::string, ClientConfig> client_configs;
    std::unordered_map<std::string, ServerConfig> server_configs;

    SECTION("parse an empty table") {
        const auto root_table = toml::parse(R"(
[test]
)");
        const auto test_table = root_table["test"].ref<toml::table>();

        parse_toml(test_table, logging_configs, client_configs, server_configs);

        CHECK(logging_configs.size() == 0);  // NOLINT
        CHECK(client_configs.size() == 0);   // NOLINT
        CHECK(server_configs.size() == 0);   // NOLINT
    }

    SECTION("parse a configuration of logging") {
        const auto root_table = toml::parse(R"(
[logging.example]
filepath = "test.log"
)");

        parse_toml(root_table, logging_configs, client_configs, server_configs);

        CHECK(logging_configs.size() == 1);  // NOLINT
        CHECK(logging_configs.at("example").filepath() == "test.log");

        CHECK(client_configs.size() == 0);  // NOLINT
        CHECK(server_configs.size() == 0);  // NOLINT
    }

    SECTION("parse a configuration of logging with invalid element type") {
        const auto root_table = toml::parse(R"(
[logging]
filepath = "test.log"
)");

        CHECK_THROWS_WITH(parse_toml(root_table, logging_configs,
                              client_configs, server_configs),
            Catch::Matchers::ContainsSubstring("logging"));
    }

    SECTION("parse a configuration of logging with invalid type") {
        const auto root_table = toml::parse(R"(
logging = "abc"
)");

        CHECK_THROWS_WITH(parse_toml(root_table, logging_configs,
                              client_configs, server_configs),
            Catch::Matchers::ContainsSubstring("logging"));
    }
}
