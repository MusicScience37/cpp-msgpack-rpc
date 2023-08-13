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
 * \brief Definition of async_connect function.
 */
#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/transport/backend_list.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "msgpack_rpc/transport/i_connector.h"

namespace msgpack_rpc::transport {

namespace impl {

/*!
 * \brief Class to establish a connection using multiple backends of protocols.
 *
 * \warning This class holds only reference to backends and URIs, and the same
 * object of this class cannot be used multiple times.
 */
class MultiBackendConnector {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] backends Backends.
     * \param[in] uris URIs.
     * \param[in] on_connection Callback function called when a connection is
     * established or error occurred in all URIs.
     */
    MultiBackendConnector(const BackendList& backends,
        const std::vector<addresses::URI>& uris,
        std::function<void(const Status&, std::shared_ptr<IConnection>)>
            on_connection)
        : backends_(&backends),
          uris_(&uris),
          on_connection_(std::move(on_connection)),
          current_uri_(uris_->cbegin()) {}

    /*!
     * \brief Start to connect to an endpoint.
     */
    void start() { async_connect(); }

    /*!
     * \brief Handle the result to connect to an endpoint.
     *
     * \param[in] status Status.
     * \param[in] connection Connection.
     */
    void operator()(
        const Status& status, std::shared_ptr<IConnection> connection) {
        if (status.code() == StatusCode::SUCCESS) {
            on_connection_(status, std::move(connection));
            return;
        }
        ++current_uri_;
        if (current_uri_ == uris_->end()) {
            on_connection_(Status(StatusCode::CONNECTION_FAILURE,
                               "Failed to connect to all the URIs."),
                std::move(connection));
            return;
        }
        async_connect();
    }

private:
    /*!
     * \brief Asynchronously connect to an endpoint.
     */
    void async_connect() {
        connector_ =
            backends_->find(current_uri_->scheme())->create_connector();
        connector_->async_connect(*current_uri_, *this);
    }

    //! Backends.
    const BackendList* backends_;

    //! URIs.
    const std::vector<addresses::URI>* uris_;

    //! Callback function called when a connection is established or error occurred in all URIs.
    std::function<void(Status, std::shared_ptr<IConnection>)> on_connection_;

    //! Current URI.
    std::vector<addresses::URI>::const_iterator current_uri_;

    //! Current connector.
    std::shared_ptr<IConnector> connector_{};
};

}  // namespace impl

/*!
 * \brief Asynchronously connect to an endpoint.
 *
 * \param[in] backends Backends.
 * \param[in] uris URIs.
 * \param[in] on_connection Callback function called when a connection is
 * established or error occurred in all URIs.
 *
 * \warning This class holds only reference to backends and URIs.
 */
void async_connect(const BackendList& backends,
    const std::vector<addresses::URI>& uris,
    std::function<void(const Status&, std::shared_ptr<IConnection>)>
        on_connection) {
    impl::MultiBackendConnector(backends, uris, std::move(on_connection))
        .start();
}

}  // namespace msgpack_rpc::transport
