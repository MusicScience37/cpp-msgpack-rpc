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
 * \brief Definition of common functions and macros to parse TOML files.
 */
#pragma once

// IWYU pragma: no_include <type_traits>

#include <chrono>     // IWYU pragma: keep
#include <exception>  // IWYU pragma: keep
#include <string>
#include <string_view>

#include <fmt/core.h>
#include <toml++/toml.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::config::toml::impl {

/*!
 * \brief Throw an exception for an error of TOML.
 *
 * \param[in] source Location in TOML.
 * \param[in] config_key Key of the configuration.
 */
[[noreturn]] inline void throw_error(
    const ::toml::source_region& source, std::string_view config_key) {
    const std::string_view source_path =
        (source.path) ? *source.path : std::string_view("(unknown file)");
    throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
        fmt::format("Invalid value for {}. (at {}:{})", config_key, source_path,
            source.begin.line));
}

/*!
 * \brief Throw an exception for an error of TOML.
 *
 * \param[in] source Location in TOML.
 * \param[in] config_key Key of the configuration.
 * \param[in] error_message Error message.
 */
[[noreturn]] inline void throw_error(const ::toml::source_region& source,
    std::string_view config_key, std::string_view error_message) {
    const std::string_view source_path =
        (source.path) ? *source.path : std::string_view("(unknown file)");
    throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
        fmt::format("Invalid value for {}: {} (at {}:{})", config_key,
            error_message, source_path, source.begin.line));
}

//! Internal macro to parse a value from TOML.
#define MSGPACK_RPC_PARSE_TOML_VALUE(KEY_STR, CONFIG_FUNCTION, TYPE) \
    const auto config_value = value.value<TYPE>();                   \
    if (!config_value) {                                             \
        throw_error(value.source(), KEY_STR);                        \
    }                                                                \
    try {                                                            \
        config.CONFIG_FUNCTION(*config_value);                       \
    } catch (const std::exception& e) {                              \
        throw_error(value.source(), KEY_STR, e.what());              \
    }

//! Internal macro to parse a value from TOML.
#define MSGPACK_RPC_PARSE_TOML_VALUE_DURATION(KEY_STR, CONFIG_FUNCTION) \
    const auto config_value = value.value<double>();                    \
    if (!config_value) {                                                \
        throw_error(value.source(), KEY_STR);                           \
    }                                                                   \
    try {                                                               \
        config.CONFIG_FUNCTION(                                         \
            std::chrono::duration_cast<std::chrono::nanoseconds>(       \
                std::chrono::duration<double>(*config_value)));         \
    } catch (const std::exception& e) {                                 \
        throw_error(value.source(), KEY_STR, e.what());                 \
    }

}  // namespace msgpack_rpc::config::toml::impl
