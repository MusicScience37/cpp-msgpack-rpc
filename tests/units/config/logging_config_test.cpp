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
 * \brief Test of LoggingConfig class.
 */
#include "msgpack_rpc/config/logging_config.h"

#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "msgpack_rpc/logging/log_level.h"

TEST_CASE("msgpack_rpc::config::LoggingConfig") {
    using msgpack_rpc::config::LoggingConfig;
    using msgpack_rpc::logging::LogLevel;

    SECTION("can be default constructible with valid configuration") {
        LoggingConfig config;

        CHECK(config.filepath() == "");  // NOLINT
        CHECK(config.max_file_size() > 0U);
        CHECK(config.max_files() > 0U);
        CHECK(config.output_log_level() == LogLevel::INFO);
    }

    SECTION("set the filepath") {
        LoggingConfig config;

        const auto value = std::string_view("test_logging_config.log");
        CHECK_NOTHROW(config.filepath(value));

        CHECK(config.filepath() == value);
    }

    SECTION("set the maximum size of a file") {
        LoggingConfig config;

        const auto value = static_cast<std::size_t>(12345);
        CHECK_NOTHROW(config.max_file_size(value));

        CHECK(config.max_file_size() == value);
    }

    SECTION("try to set the maximum size of a file to an invalid value") {
        LoggingConfig config;

        const auto value = static_cast<std::size_t>(0);
        CHECK_THROWS(config.max_file_size(value));
    }

    SECTION("set the maximum number of files") {
        LoggingConfig config;

        const auto value = static_cast<std::size_t>(12345);
        CHECK_NOTHROW(config.max_files(value));

        CHECK(config.max_files() == value);
    }

    SECTION("try to set the maximum number of files to an invalid value") {
        LoggingConfig config;

        const auto value = static_cast<std::size_t>(0);
        CHECK_THROWS(config.max_files(value));
    }

    SECTION("set the log level to write logs") {
        LoggingConfig config;

        const auto value =
            GENERATE(LogLevel::TRACE, LogLevel::DEBUG, LogLevel::INFO,
                LogLevel::WARN, LogLevel::ERROR, LogLevel::CRITICAL);
        CHECK_NOTHROW(config.output_log_level(value));

        CHECK(config.output_log_level() == value);
    }

    SECTION("set the log level to write logs to an invalid value") {
        LoggingConfig config;

        const auto value =
            static_cast<LogLevel>(static_cast<int>(LogLevel::CRITICAL) + 1);
        CHECK_THROWS(config.output_log_level(value));
    }
}
