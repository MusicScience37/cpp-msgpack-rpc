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
 * \brief Definition of BackendList class.
 */
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <fmt/core.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/transport/i_backend.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of lists of backends of protocols.
 */
class BackendList {
public:
    /*!
     * \brief Constructor.
     */
    BackendList() = default;

    /*!
     * \brief Add a backend of the protocol.
     *
     * \param[in] backend Backend of the protocol.
     */
    void append(std::shared_ptr<transport::IBackend> backend) {
        const auto scheme = backend->scheme();
        const auto result = backends_.try_emplace(scheme, std::move(backend));
        if (!result.second) {
            throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
                fmt::format("Duplicate scheme : \"{}\".", scheme));
        }
    }

    /*!
     * \brief Find a backend. Throw an exception when not found.
     *
     * \param[in] scheme Scheme.
     * \return Backend of the protocol with the given scheme.
     */
    [[nodiscard]] std::shared_ptr<transport::IBackend> find(
        std::string_view scheme) const {
        const auto backend_iter = backends_.find(scheme);
        if (backend_iter == backends_.end()) {
            throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
                fmt::format("Invalid scheme: {}.", scheme));
        }
        return backend_iter->second;
    }

private:
    //! Backends.
    std::unordered_map<std::string_view, std::shared_ptr<transport::IBackend>>
        backends_{};
};

}  // namespace msgpack_rpc::transport
