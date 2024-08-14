/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of get_errno_message function.
 */
#pragma once

#include <string>
#include <system_error>

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Get the error message for an error number.
 *
 * \param[in] errno_val Error number.
 * \return Error message.
 *
 * \note This function can accept values set to `errno` variable.
 */
[[nodiscard]] inline std::string get_errno_message(int errno_val) {
    return std::generic_category().message(errno_val);
}

}  // namespace msgpack_rpc::transport::posix_shm
