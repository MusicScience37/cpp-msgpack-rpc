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
 * \brief Implementation of format_msgpack_object_to_string function.
 */
#include "msgpack_rpc/util/format_msgpack_object_to_string.h"

#include <iterator>
#include <string>

#include <fmt/base.h>
#include <fmt/format.h>
#include <msgpack.hpp>

#include "msgpack_rpc/util/format_msgpack_object_to.h"

namespace msgpack_rpc::util {

[[nodiscard]] std::string format_msgpack_object_to_string(
    const msgpack::object& object) {
    fmt::memory_buffer buffer;
    format_msgpack_object_to(std::back_inserter(buffer), object);
    return std::string(buffer.data(), buffer.size());
}

}  // namespace msgpack_rpc::util
