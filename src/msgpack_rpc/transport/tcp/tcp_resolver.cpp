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
 * \brief Implementation of TCPResolver class.
 */
#include "msgpack_rpc/transport/tcp/tcp_resolver.h"

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::transport::tcp {

TCPResolver::TCPResolver(executors::AsioContextType& context,
    std::shared_ptr<logging::Logger> logger)
    : impl_(context, "Resolver(TCP)", std::move(logger)) {}

std::vector<addresses::Address> TCPResolver::resolve(
    const addresses::URI& uri) {
    if (uri.schema() != addresses::TCP_SCHEME) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            fmt::format(
                "Scheme is different with the resolver: expected={}, actual={}",
                addresses::TCP_SCHEME, uri.schema()));
    }
    return impl_.resolve(uri);
}

TCPResolver::~TCPResolver() = default;

}  // namespace msgpack_rpc::transport::tcp
