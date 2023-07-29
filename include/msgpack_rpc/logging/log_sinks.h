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
 * \brief Declaration of functions to create log sinks.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <string_view>

#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/logging/i_log_sink.h"

namespace msgpack_rpc::logging {

/*!
 * \brief Create a log sink to write to standard output.
 *
 * \return Log sink.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<ILogSink>
create_stdout_log_sink();

/*!
 * \brief Create a log sink to write to a file with rotation.
 *
 * \param[in] filepath Filepath.
 * \param[in] max_file_size Maximum size of a file.
 * \param[in] max_files Maximum number of files.
 * \return Log sink.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<ILogSink>
create_rotating_file_log_sink(std::string_view filepath,
    std::size_t max_file_size, std::size_t max_files);

/*!
 * \brief Create a log sink from a configuration.
 *
 * \param[in] config Configuration of logging.
 * \return Log sink.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<ILogSink>
create_log_sink_from_config(const config::LoggingConfig& config);

}  // namespace msgpack_rpc::logging
