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
 * \brief Implementation of MsgpackRPCException class.
 */
#include "msgpack_rpc/common/msgpack_rpc_exception.h"

#include <utility>

namespace msgpack_rpc {
inline namespace common {

MsgpackRPCException::MsgpackRPCException(Status status)
    : std::runtime_error(status.message().data()), status_(std::move(status)) {}

MsgpackRPCException::MsgpackRPCException(
    StatusCode code, std::string_view message)
    : MsgpackRPCException(Status(code, message)) {}

const Status& MsgpackRPCException::status() const noexcept { return status_; }

MsgpackRPCException::MsgpackRPCException(
    const MsgpackRPCException&) noexcept = default;

MsgpackRPCException::MsgpackRPCException(
    MsgpackRPCException&&) noexcept = default;

MsgpackRPCException& MsgpackRPCException::operator=(
    const MsgpackRPCException&) noexcept = default;
MsgpackRPCException& MsgpackRPCException::operator=(
    MsgpackRPCException&&) noexcept = default;

MsgpackRPCException::~MsgpackRPCException() = default;

}  // namespace common
}  // namespace msgpack_rpc
