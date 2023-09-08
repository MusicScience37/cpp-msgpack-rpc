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
#include "msgpack_rpc/config/toml/parse_toml.h"

#include <approval_test_fmt.h>
#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/log_level.h"

TEST_CASE("msgpack_rpc::config::toml::impl::throw_error") {
    using msgpack_rpc::MsgpackRPCException;
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::config::toml::impl::throw_error;

    SECTION("throw with a path") {
        constexpr std::string_view source_path = "test.toml";
        const auto root_table = toml::parse(R"(
[test]
example = 12345
)",
            source_path);
        const auto value = root_table.at_path("test.example");

        SECTION("and without a message") {
            try {
                throw_error(value.node()->source(), "example");
                FAIL();
            } catch (const MsgpackRPCException& e) {
                CHECK(e.status().code() == StatusCode::INVALID_ARGUMENT);
                ApprovalTests::Approvals::verify(e.status().message());
            }
        }

        SECTION("and with message") {
            try {
                throw_error(value.node()->source(), "example", "message");
                FAIL();
            } catch (const MsgpackRPCException& e) {
                CHECK(e.status().code() == StatusCode::INVALID_ARGUMENT);
                ApprovalTests::Approvals::verify(e.status().message());
            }
        }
    }

    SECTION("throw without a path") {
        const auto root_table = toml::parse(R"(
[test]
example = 12345
)");
        const auto value = root_table.at_path("test.example");

        SECTION("and without a message") {
            try {
                throw_error(value.node()->source(), "example");
                FAIL();
            } catch (const MsgpackRPCException& e) {
                CHECK(e.status().code() == StatusCode::INVALID_ARGUMENT);
                ApprovalTests::Approvals::verify(e.status().message());
            }
        }

        SECTION("and with message") {
            try {
                throw_error(value.node()->source(), "example", "message");
                FAIL();
            } catch (const MsgpackRPCException& e) {
                CHECK(e.status().code() == StatusCode::INVALID_ARGUMENT);
                ApprovalTests::Approvals::verify(e.status().message());
            }
        }
    }
}

TEST_CASE("msgpack_rpc::config::toml::impl::parse_toml(LoggingConfig)") {
    using msgpack_rpc::config::toml::impl::parse_toml;
    using msgpack_rpc::logging::LogLevel;

    SECTION("parse an empty table") {
        const auto root_table = toml::parse(R"(
[test]
)");
        const auto test_table = root_table["test"].ref<toml::table>();
        msgpack_rpc::config::LoggingConfig config;

        REQUIRE_NOTHROW(parse_toml(test_table, config));
    }

    SECTION("parse filepath") {
        const auto root_table = toml::parse(R"(
[test]
filepath = "test.log"
)");
        const auto test_table = root_table["test"].ref<toml::table>();
        msgpack_rpc::config::LoggingConfig config;

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.filepath() == "test.log");
    }

    SECTION("parse max_file_size") {
        const auto root_table = toml::parse(R"(
[test]
max_file_size = 12345
)");
        const auto test_table = root_table["test"].ref<toml::table>();
        msgpack_rpc::config::LoggingConfig config;

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.max_file_size() == 12345);
    }

    SECTION("parse max_files") {
        const auto root_table = toml::parse(R"(
[test]
max_files = 123
)");
        const auto test_table = root_table["test"].ref<toml::table>();
        msgpack_rpc::config::LoggingConfig config;

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.max_files() == 123);
    }

    SECTION("parse output_log_level") {
        const auto root_table = toml::parse(R"(
[test]
output_log_level = "debug"
)");
        const auto test_table = root_table["test"].ref<toml::table>();
        msgpack_rpc::config::LoggingConfig config;

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.output_log_level() == LogLevel::DEBUG);
    }
}

TEST_CASE("msgpack_rpc::config::toml::impl::parse_toml(MessageParserConfig)") {
    using msgpack_rpc::config::toml::impl::parse_toml;

    SECTION("parse an empty table") {
        const auto root_table = toml::parse(R"(
[test]
)");
        const auto test_table = root_table["test"].ref<toml::table>();
        msgpack_rpc::config::MessageParserConfig config;

        REQUIRE_NOTHROW(parse_toml(test_table, config));
    }

    SECTION("parse read_buffer_size") {
        const auto root_table = toml::parse(R"(
[test]
read_buffer_size = 12345
)");
        const auto test_table = root_table["test"].ref<toml::table>();
        msgpack_rpc::config::MessageParserConfig config;

        REQUIRE_NOTHROW(parse_toml(test_table, config));

        CHECK(config.read_buffer_size() == 12345);
    }
}

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
}
