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
 * \brief Definition of ConnectionList class.
 */
#pragma once

#include <memory>
#include <mutex>
#include <unordered_set>
#include <utility>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of lists of connections.
 *
 * \tparam Connection Type of connections.
 */
template <typename Connection>
class ConnectionList {
public:
    /*!
     * \brief Constructor.
     */
    ConnectionList() = default;

    /*!
     * \brief Add a connection.
     *
     * \param[in] connection Connection.
     */
    void append(std::shared_ptr<Connection> connection) {
        std::unique_lock<std::mutex> lock(mutex_);
        list_.insert(std::move(connection));
    }

    /*!
     * \brief Remove a connection.
     *
     * \param[in] connection Connection.
     */
    void remove(const std::shared_ptr<Connection>& connection) {
        std::unique_lock<std::mutex> lock(mutex_);
        list_.erase(connection);
    }

    /*!
     * \brief Asynchronously close all connections.
     */
    void async_close_all() {
        std::unique_lock<std::mutex> lock(mutex_);
        for (const auto& connection : list_) {
            connection->async_close();
        }
    }

private:
    //! List of connections.
    std::unordered_set<std::shared_ptr<Connection>> list_{};

    //! Mutex.
    std::mutex mutex_{};
};

}  // namespace msgpack_rpc::transport
