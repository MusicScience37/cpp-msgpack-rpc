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
 * \brief Implementation of PosixSharedMemoryAddress class.
 */
#include "msgpack_rpc/addresses/posix_shared_memory_address.h"

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_SHM

#include <ostream>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "msgpack_rpc/addresses/schemes.h"
#include "msgpack_rpc/addresses/uri.h"

namespace msgpack_rpc::addresses {

PosixSharedMemoryAddress::PosixSharedMemoryAddress(std::string file_name)
    : file_name_(std::move(file_name)) {
    if (file_name_.find_first_of('/') != std::string::npos) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "File name of the shared memory must not contain slashes.");
    }
}

const std::string& PosixSharedMemoryAddress::file_name() const noexcept {
    return file_name_;
}

URI PosixSharedMemoryAddress::to_uri() const {
    return URI(SHARED_MEMORY_SCHEME, file_name_);
}

std::string PosixSharedMemoryAddress::to_string() const {
    return fmt::format("{}", *this);
}

bool PosixSharedMemoryAddress::operator==(
    const PosixSharedMemoryAddress& right) const {
    return file_name_ == right.file_name_;
}

bool PosixSharedMemoryAddress::operator!=(
    const PosixSharedMemoryAddress& right) const {
    return !operator==(right);
}

std::ostream& operator<<(std::ostream& stream,
    const msgpack_rpc::addresses::PosixSharedMemoryAddress& address) {
    fmt::print(stream, "{}", address);
    return stream;
}

}  // namespace msgpack_rpc::addresses

namespace fmt {

format_context::iterator
formatter<msgpack_rpc::addresses::PosixSharedMemoryAddress>::format(  // NOLINT
    const msgpack_rpc::addresses::PosixSharedMemoryAddress& val,
    format_context& context) const {
    return fmt::format_to(context.out(), "shm://{}", val.file_name());
}

}  // namespace fmt

#endif
