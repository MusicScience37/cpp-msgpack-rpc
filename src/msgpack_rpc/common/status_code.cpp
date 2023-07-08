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
 * \brief Implementation of format_status_code.
 */
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc {
inline namespace common {

std::string_view format_status_code(StatusCode code) noexcept {
    switch (code) {
    case StatusCode::SUCCESS:
        return "SUCCESS";
    case StatusCode::INVALID_ARGUMENT:
        return "INVALID_ARGUMENT";
    case StatusCode::INVALID_MESSAGE:
        return "INVALID_MESSAGE";
    }
    return "INVALID_STATUS_CODE";
}

}  // namespace common
}  // namespace msgpack_rpc
