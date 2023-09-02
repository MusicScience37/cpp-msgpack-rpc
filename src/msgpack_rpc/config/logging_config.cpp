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
 * \brief Implementation of LoggingConfig class.
 */
#include "msgpack_rpc/config/logging_config.h"

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/logging/log_level.h"

namespace msgpack_rpc::config {

namespace {

//! Default maximum size of a file.
constexpr auto LOGGING_CONFIG_DEFAULT_MAX_FILE_SIZE =
    static_cast<std::size_t>(1024 * 1024);  // 1 MiB.

//! Default maximum size of a file.
constexpr auto LOGGING_CONFIG_DEFAULT_MAX_FILES = static_cast<std::size_t>(5);

//! Default log level to write logs.
constexpr auto LOGGING_CONFIG_DEFAULT_OUTPUT_LOG_LEVEL =
    logging::LogLevel::INFO;

}  // namespace

LoggingConfig::LoggingConfig()
    : max_file_size_(LOGGING_CONFIG_DEFAULT_MAX_FILE_SIZE),
      max_files_(LOGGING_CONFIG_DEFAULT_MAX_FILES),
      output_log_level_(LOGGING_CONFIG_DEFAULT_OUTPUT_LOG_LEVEL) {}

LoggingConfig& LoggingConfig::filepath(std::string_view value) {
    filepath_ = value;
    return *this;
}

LoggingConfig& LoggingConfig::max_file_size(std::size_t value) {
    if (value <= 0U) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Maximum size of a file must be greater than 0.");
    }
    max_file_size_ = value;
    return *this;
}

LoggingConfig& LoggingConfig::max_files(std::size_t value) {
    if (value <= 0U) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Maximum number of files must be greater than 0.");
    }
    max_files_ = value;
    return *this;
}

LoggingConfig& LoggingConfig::output_log_level(logging::LogLevel value) {
    switch (value) {
    case logging::LogLevel::TRACE:
    case logging::LogLevel::DEBUG:
    case logging::LogLevel::INFO:
    case logging::LogLevel::WARN:
    case logging::LogLevel::ERROR:
    case logging::LogLevel::CRITICAL:
        break;
    default:
        throw MsgpackRPCException(
            StatusCode::INVALID_ARGUMENT, "Invalid log level.");
    }
    output_log_level_ = value;
    return *this;
}

std::string_view LoggingConfig::filepath() const noexcept { return filepath_; }

std::size_t LoggingConfig::max_file_size() const noexcept {
    return max_file_size_;
}

std::size_t LoggingConfig::max_files() const noexcept { return max_files_; }

logging::LogLevel LoggingConfig::output_log_level() const noexcept {
    return output_log_level_;
}

}  // namespace msgpack_rpc::config
