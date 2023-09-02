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
 * \brief Definition of LoggingConfig class.
 */
#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/logging/log_level.h"

namespace msgpack_rpc::config {

/*!
 * \brief Class of Logging configuration.
 */
class MSGPACK_RPC_EXPORT LoggingConfig {
public:
    /*!
     * \brief Constructor.
     */
    LoggingConfig();

    /*!
     * \brief Set the filepath.
     *
     * \param[in] value Value.
     * \return This.
     */
    LoggingConfig& filepath(std::string_view value);

    /*!
     * \brief Set the maximum size of a file.
     *
     * \param[in] value Value.
     * \return This.
     */
    LoggingConfig& max_file_size(std::size_t value);

    /*!
     * \brief Set the maximum number of files.
     *
     * \param[in] value Value.
     * \return This.
     */
    LoggingConfig& max_files(std::size_t value);

    /*!
     * \brief Set the log level to write logs.
     *
     * \param[in] value Value.
     * \return This.
     */
    LoggingConfig& output_log_level(logging::LogLevel value);

    /*!
     * \brief Get the filepath.
     *
     * \return Filepath.
     *
     * \note An empty string specifies that logs should be written to STDOUT.
     */
    [[nodiscard]] std::string_view filepath() const noexcept;

    /*!
     * \brief Get the maximum size of a file.
     *
     * \return Maximum size of a file.
     */
    [[nodiscard]] std::size_t max_file_size() const noexcept;

    /*!
     * \brief Get the maximum number of files.
     *
     * \return Maximum number of files.
     */
    [[nodiscard]] std::size_t max_files() const noexcept;

    /*!
     * \brief Get the log level to write logs.
     *
     * \return Log level to write logs.
     */
    [[nodiscard]] logging::LogLevel output_log_level() const noexcept;

private:
    //! Filepath.
    std::string filepath_;

    //! Maximum size of a file.
    std::size_t max_file_size_;

    //! Maximum number of files.
    std::size_t max_files_;

    //! Log level to write logs.
    logging::LogLevel output_log_level_;
};

}  // namespace msgpack_rpc::config
