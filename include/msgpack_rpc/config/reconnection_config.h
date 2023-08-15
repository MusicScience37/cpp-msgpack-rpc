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
     * \brief Set the initial waiting time.
     *
     * \param[in] value Value.
     * \return This.
     */
    ReconnectionConfig& initial_waiting_time(std::chrono::nanoseconds value);

    /*!
     * \brief Get the initial waiting time.
     *
     * \return Value.
     */
    [[nodiscard]] std::chrono::nanoseconds initial_waiting_time()
        const noexcept;

    /*!
     * \brief Set the maximum waiting time.
     *
     * \param[in] value Value.
     * \return This.
     */
    ReconnectionConfig& max_waiting_time(std::chrono::nanoseconds value);

    /*!
     * \brief Get the maximum waiting time.
     *
     * \return Value.
     */
    [[nodiscard]] std::chrono::nanoseconds max_waiting_time() const noexcept;

    /*!
     * \brief Set the maximum jitter of waiting time.
     *
     * \param[in] value Value.
     * \return This.
     */
    ReconnectionConfig& max_jitter_waiting_time(std::chrono::nanoseconds value);

    /*!
     * \brief Get the maximum jitter of waiting time.
     *
     * \return Value.
     */
    [[nodiscard]] std::chrono::nanoseconds max_jitter_waiting_time()
        const noexcept;

private:
    //! Initial waiting time.
    std::chrono::nanoseconds initial_waiting_time_;

    //! Maximum waiting time.
    std::chrono::nanoseconds max_waiting_time_;

    //! Maximum jitter of waiting time.
    std::chrono::nanoseconds max_jitter_waiting_time_;
};

}  // namespace msgpack_rpc::config
