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
 * \brief Definition of SpdlogLogSink class.
 */
#pragma once

#include <spdlog/spdlog.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/logging/i_log_sink.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/source_location_view.h"

namespace msgpack_rpc::logging::impl::spdlog_backend {

/*!
 * \brief Convert a log level in this library to a log level in spdlog library.
 *
 * \param[in] level Log level in this library.
 * \return Log level in spdlog library.
 */
[[nodiscard]] inline spdlog::level::level_enum convert_log_level(
    LogLevel level) {
    using spdlog::level::level_enum;
    switch (level) {
    case LogLevel::TRACE:
        return level_enum::trace;
    case LogLevel::DEBUG:
        return level_enum::debug;
    case LogLevel::INFO:
        return level_enum::info;
    case LogLevel::WARN:
        return level_enum::warn;
    case LogLevel::ERROR:
        return level_enum::err;
    case LogLevel::CRITICAL:
        return level_enum::critical;
    }
    return level_enum::trace;
}

/*!
 * \brief Class of log sinks using spdlog library.
 */
class SpdlogLogSink final : public ILogSink {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger in spdlog library.
     */
    explicit SpdlogLogSink(std::shared_ptr<spdlog::logger> logger)
        : logger_(std::move(logger)) {
        if (!logger_) {
            throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
                "Null logger was given to SpdlogLogSink class.");
        }

        // Log level is checked in Logger class.
        logger_->set_level(spdlog::level::trace);
    }

    //! \copydoc msgpack_rpc::logging::ILogSink::write
    void write(SourceLocationView location, LogLevel level,
        std::string_view body) override {
        logger_->log(
            spdlog::source_loc(location.filepath().data(),
                static_cast<int>(location.line()), location.function().data()),
            convert_log_level(level), body);
        logger_->flush();  // TODO configuration
    }

private:
    //! Logger in spdlog library.
    std::shared_ptr<spdlog::logger> logger_;
};

/*!
 * \brief Configure the log format of a logger in spdlog library for output to
 * consoles.
 *
 * \param[in] logger Logger in spdlog library.
 */
inline void configure_spdlog_logger_format_for_consoles(
    const std::shared_ptr<spdlog::logger>& logger) {
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v (%s:%#, %!)");
}

/*!
 * \brief Configure the log format of a logger in spdlog library for output to
 * files.
 *
 * \param[in] logger Logger in spdlog library.
 */
inline void configure_spdlog_logger_format_for_files(
    const std::shared_ptr<spdlog::logger>& logger) {
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%l] %v (%@, %!)");
}

}  // namespace msgpack_rpc::logging::impl::spdlog_backend
