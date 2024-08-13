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
 * \brief Definition of create_test_logger function.
 */
#include "create_test_logger.h"

#include <cstddef>
#include <memory>
#include <string_view>

#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/log_sinks.h"
#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc_test {

std::shared_ptr<msgpack_rpc::logging::Logger> create_test_logger() {
    static constexpr auto file_path =
        std::string_view("msgpack_rpc_test_units.log");
    static constexpr auto max_file_size = static_cast<std::size_t>(1024 * 1024);
    static constexpr auto max_files = static_cast<std::size_t>(3);
    static const auto logger = msgpack_rpc::logging::Logger::create(
        msgpack_rpc::logging::create_rotating_file_log_sink(
            file_path, max_file_size, max_files),
        msgpack_rpc::logging::LogLevel::TRACE);
    return logger;
}

}  // namespace msgpack_rpc_test
