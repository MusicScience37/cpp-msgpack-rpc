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
 * \brief Implementation of ServerException class.
 */
#include "msgpack_rpc/clients/server_exception.h"

#include <utility>

namespace msgpack_rpc::clients {

ServerException::ServerException(
    msgpack::object object, std::shared_ptr<msgpack::zone> zone)
    : MsgpackRPCException(StatusCode::SERVER_ERROR, "An error in a server."),
      zone_(std::move(zone)),
      object_(object) {}

const msgpack::object& ServerException::object() const noexcept {
    return object_;
}

ServerException::ServerException(const ServerException&) noexcept = default;

ServerException::ServerException(ServerException&&) noexcept = default;

ServerException& ServerException::operator=(
    const ServerException&) noexcept = default;

ServerException& ServerException::operator=(
    ServerException&&) noexcept = default;

ServerException::~ServerException() noexcept = default;

}  // namespace msgpack_rpc::clients
