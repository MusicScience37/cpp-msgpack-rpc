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
 * \brief Definition of StatusCode enumeration.
 */
#pragma once

#include <string_view>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc {
inline namespace common {

/*!
 * \brief Enumeration of status codes.
 */
enum class StatusCode {
    //! Success.
    SUCCESS = 0,

    //! Invalid arguments in functions.
    INVALID_ARGUMENT,

    //! Failure in parsing a message.
    INVALID_MESSAGE,

    //! Invalid types in a message.
    TYPE_ERROR,

    //! Pre-condition for a process is not met.
    PRECONDITION_NOT_MET,
};

/*!
 * \brief Format a status code.
 *
 * \param[in] code Status code.
 * \return Formatted string.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::string_view format_status_code(
    StatusCode code) noexcept;

}  // namespace common
}  // namespace msgpack_rpc
