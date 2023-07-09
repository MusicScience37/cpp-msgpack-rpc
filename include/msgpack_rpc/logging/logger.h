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
 * \brief Definition of Logger class.
 */
#pragma once

#include <iterator>
#include <memory>
#include <utility>

#include <fmt/format.h>

#include "msgpack_rpc/logging/i_log_sink.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/source_location_view.h"

namespace msgpack_rpc::logging {

/*!
 * \brief Class to write logs.
 *
 * \note This class is assumed to be used in logging macros.
 */
class Logger {
public:
    /*!
     * \brief Create a logger.
     *
     * \param[in] sink Log sink.
     * \param[in] output_log_level Log level to write logs.
     * \return Logger.
     */
    [[nodiscard]] static std::shared_ptr<Logger> create(
        std::shared_ptr<ILogSink> sink,
        LogLevel output_log_level = LogLevel::INFO) {
        return std::make_shared<Logger>(std::move(sink), output_log_level);
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] sink Log sink.
     * \param[in] output_log_level Log level to write logs.
     */
    Logger(std::shared_ptr<ILogSink> sink, LogLevel output_log_level)
        : sink_(std::move(sink)), output_log_level_(output_log_level) {}

    /*!
     * \brief Write a log.
     *
     * \tparam Body Type of the body of the log. (Must be convertible to
     * std::string_view.)
     * \param[in] location Location in source codes.
     * \param[in] level Log level.
     * \param[in] body Body of the log.
     */
    template <typename Body>
    void write(SourceLocationView location, LogLevel level, Body&& body) {
        if (sink_) {
            sink_->write(
                location, level, std::string_view(std::forward<Body>(body)));
        }
    }

    /*!
     * \brief Write a log with formatting.
     *
     * \tparam Args Type of arguments of the format of the body.
     * \param[in] location Location in source codes.
     * \param[in] level Log level.
     * \param[in] format Format of the body.
     * \param[in] args Arguments of the format of the body.
     */
    template <typename... Args>
    void write(SourceLocationView location, LogLevel level,
        fmt::format_string<Args...> format, Args&&... args) {
        if (sink_) {
            fmt::memory_buffer buffer;
            fmt::format_to(std::back_inserter(buffer), format,
                std::forward<Args>(args)...);
            sink_->write(location, level,
                std::string_view(buffer.data(), buffer.size()));
        }
    }

    /*!
     * \brief Get the log level to write logs.
     *
     * \return Log level.
     */
    [[nodiscard]] LogLevel output_log_level() const noexcept {
        return output_log_level_;
    }

private:
    //! Log sink.
    std::shared_ptr<ILogSink> sink_;

    //! Log level.
    LogLevel output_log_level_;
};

}  // namespace msgpack_rpc::logging

/*!
 * \brief Write a log.
 *
 * \param[in] LOGGER_PTR Pointer to the logger.
 * \param[in] LEVEL Log level.
 *
 * Remaining arguments are the format of the log body and its arguments, or the
 * log body itself.
 */
#define MSGPACK_RPC_LOG(LOGGER_PTR, LEVEL, ...)                       \
    do {                                                              \
        if (LEVEL >= (LOGGER_PTR)->output_log_level()) {              \
            (LOGGER_PTR)                                              \
                ->write(MSGPACK_RPC_CURRENT_SOURCE_LOCATION(), LEVEL, \
                    __VA_ARGS__);                                     \
        }                                                             \
    } while (false)

/*!
 * \brief Write a trace log.
 *
 * \param[in] LOGGER_PTR Pointer to the logger.
 *
 * Remaining arguments are the format of the log body and its arguments, or the
 * log body itself.
 */
#define MSGPACK_RPC_TRACE(LOGGER_PTR, ...) \
    MSGPACK_RPC_LOG(                       \
        LOGGER_PTR, ::msgpack_rpc::logging::LogLevel::TRACE, __VA_ARGS__)

/*!
 * \brief Write a debug log.
 *
 * \param[in] LOGGER_PTR Pointer to the logger.
 *
 * Remaining arguments are the format of the log body and its arguments, or the
 * log body itself.
 */
#define MSGPACK_RPC_DEBUG(LOGGER_PTR, ...) \
    MSGPACK_RPC_LOG(                       \
        LOGGER_PTR, ::msgpack_rpc::logging::LogLevel::DEBUG, __VA_ARGS__)

/*!
 * \brief Write a information log.
 *
 * \param[in] LOGGER_PTR Pointer to the logger.
 *
 * Remaining arguments are the format of the log body and its arguments, or the
 * log body itself.
 */
#define MSGPACK_RPC_INFO(LOGGER_PTR, ...) \
    MSGPACK_RPC_LOG(                      \
        LOGGER_PTR, ::msgpack_rpc::logging::LogLevel::INFO, __VA_ARGS__)

/*!
 * \brief Write a warning log.
 *
 * \param[in] LOGGER_PTR Pointer to the logger.
 *
 * Remaining arguments are the format of the log body and its arguments, or the
 * log body itself.
 */
#define MSGPACK_RPC_WARN(LOGGER_PTR, ...) \
    MSGPACK_RPC_LOG(                      \
        LOGGER_PTR, ::msgpack_rpc::logging::LogLevel::WARN, __VA_ARGS__)

/*!
 * \brief Write a error log.
 *
 * \param[in] LOGGER_PTR Pointer to the logger.
 *
 * Remaining arguments are the format of the log body and its arguments, or the
 * log body itself.
 */
#define MSGPACK_RPC_ERROR(LOGGER_PTR, ...) \
    MSGPACK_RPC_LOG(                       \
        LOGGER_PTR, ::msgpack_rpc::logging::LogLevel::ERROR, __VA_ARGS__)

/*!
 * \brief Write a critical log.
 *
 * \param[in] LOGGER_PTR Pointer to the logger.
 *
 * Remaining arguments are the format of the log body and its arguments, or the
 * log body itself.
 */
#define MSGPACK_RPC_CRITICAL(LOGGER_PTR, ...) \
    MSGPACK_RPC_LOG(                          \
        LOGGER_PTR, ::msgpack_rpc::logging::LogLevel::CRITICAL, __VA_ARGS__)
