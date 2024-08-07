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
 * \brief Definition of Server class.
 */
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/servers/impl/i_server_impl.h"

namespace msgpack_rpc::servers {

/*!
 * \brief Class of servers.
 */
class Server {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] impl Object of the internal implementation.
     */
    explicit Server(std::unique_ptr<impl::IServerImpl> impl) noexcept
        : impl_(std::move(impl)) {}

    /*!
     * \brief Stop processing of this server.
     */
    void stop() { impl_->stop(); }

    /*!
     * \brief Run processing of this server until a signal is received.
     */
    void run_until_signal() { impl_->run_until_signal(); }

    /*!
     * \brief Get the URIs of the local endpoints in this server.
     *
     * \return URIs.
     */
    [[nodiscard]] std::vector<addresses::URI> local_endpoint_uris() {
        return impl_->local_endpoint_uris();
    }

    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     *
     * \note This function is mainly for testing. So this function may be
     * removed in the future.
     */
    [[nodiscard]] std::shared_ptr<executors::IExecutor> executor() {
        return impl_->executor();
    }

private:
    //! Object of the internal implementation.
    std::unique_ptr<impl::IServerImpl> impl_;
};

}  // namespace msgpack_rpc::servers
