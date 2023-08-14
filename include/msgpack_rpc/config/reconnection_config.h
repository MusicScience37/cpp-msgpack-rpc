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
 * \brief Definition of ReconnectionConfig class.
 */
#pragma once

#include <chrono>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::config {

/*!
 * \brief Class of configurations of reconnection.
 */
class MSGPACK_RPC_EXPORT ReconnectionConfig {
public:
    /*!
     * \brief Constructor.
     */
    ReconnectionConfig();

    /*!
     * \brief Set the time before reconnecting.
     *
     * \param[in] value Value.
     * \return This.
     */
    ReconnectionConfig& wait_time(std::chrono::nanoseconds value);

    /*!
     * \brief Get the time before reconnecting.
     *
     * \return Time before reconnecting.
     */
    [[nodiscard]] std::chrono::nanoseconds wait_time() const noexcept;

private:
    //! Time before reconnecting.
    std::chrono::nanoseconds wait_time_;
};

}  // namespace msgpack_rpc::config
