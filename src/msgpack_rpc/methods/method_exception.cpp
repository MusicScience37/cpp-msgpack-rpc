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
 * \brief Implementation of MethodException class.
 */
#include "msgpack_rpc/methods/method_exception.h"

namespace msgpack_rpc::methods {

const msgpack::object& MethodException::object() const noexcept {
    return object_;
}

const char* MethodException::what() const noexcept {
    return "Method threw an exception.";
}

MethodException::MethodException(const MethodException&) noexcept = default;

MethodException::MethodException(MethodException&&) noexcept = default;

MethodException& MethodException::operator=(
    const MethodException&) noexcept = default;

MethodException& MethodException::operator=(
    MethodException&&) noexcept = default;

MethodException::~MethodException() noexcept = default;

}  // namespace msgpack_rpc::methods
