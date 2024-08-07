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
 * \brief Definition of IServerImpl class.
 */
#pragma once

#include <memory>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/executors/i_executor.h"

namespace msgpack_rpc::servers::impl {

/*!
 * \brief Interface of internal implementation of servers.
 */
class IServerImpl {
public:
    /*!
     * \brief Stop processing of this server.
     */
    virtual void stop() = 0;

    /*!
     * \brief Run processing of this server until a signal is received.
     */
    virtual void run_until_signal() = 0;

    /*!
     * \brief Get the URIs of the local endpoints in this server.
     *
     * \return URIs.
     */
    [[nodiscard]] virtual std::vector<addresses::URI> local_endpoint_uris() = 0;

    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     *
     * \note This function is mainly for testing. So this function may be
     * removed in the future.
     */
    [[nodiscard]] virtual std::shared_ptr<executors::IExecutor> executor() = 0;

    IServerImpl(const IServerImpl&) = delete;
    IServerImpl(IServerImpl&&) = delete;
    IServerImpl& operator=(const IServerImpl&) = delete;
    IServerImpl& operator=(IServerImpl&&) = delete;

    //! Destructor.
    virtual ~IServerImpl() noexcept = default;

protected:
    //! Constructor.
    IServerImpl() noexcept = default;
};

}  // namespace msgpack_rpc::servers::impl
