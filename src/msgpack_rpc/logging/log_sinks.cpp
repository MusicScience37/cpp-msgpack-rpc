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
 * \brief Implementation of functions to create log sinks.
 */
#include "msgpack_rpc/logging/log_sinks.h"

#include <cstddef>
#include <memory>
#include <string_view>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "msgpack_rpc/logging/i_log_sink.h"
#include "msgpack_rpc/logging/impl/spdlog_log_sink.h"

namespace msgpack_rpc::logging {

std::shared_ptr<ILogSink> create_stdout_log_sink_impl() {
    auto spdlog_logger = spdlog::stdout_color_mt("stdout");
    impl::spdlog_backend::configure_spdlog_logger_format_for_consoles(
        spdlog_logger);
    return std::make_shared<impl::spdlog_backend::SpdlogLogSink>(
        std::move(spdlog_logger));
}

std::shared_ptr<ILogSink> create_stdout_log_sink() {
    static const auto log_sink = create_stdout_log_sink_impl();
    return log_sink;
}

std::shared_ptr<ILogSink> create_rotating_file_log_sink(
    std::string_view filepath, std::size_t max_file_size,
    std::size_t max_files) {
    auto spdlog_logger = spdlog::rotating_logger_mt(std::string(filepath),
        spdlog::filename_t(filepath), max_file_size, max_files, true);
    impl::spdlog_backend::configure_spdlog_logger_format_for_files(
        spdlog_logger);
    return std::make_shared<impl::spdlog_backend::SpdlogLogSink>(
        std::move(spdlog_logger));
}

std::shared_ptr<ILogSink> create_log_sink_from_config(
    const config::LoggingConfig& config) {
    if (config.filepath().empty()) {
        return create_stdout_log_sink();
    }
    return create_rotating_file_log_sink(
        config.filepath(), config.max_file_size(), config.max_files());
}

}  // namespace msgpack_rpc::logging
