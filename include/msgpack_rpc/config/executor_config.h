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
 * \brief Definition of ExecutorConfig class.
 */
#pragma once

#include <cstddef>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::config {

/*!
 * \brief Class of configuration of executors.
 */
class MSGPACK_RPC_EXPORT ExecutorConfig {
public:
    //! Constructor.
    ExecutorConfig();

    /*!
     * \brief Set the number of threads for transport.
     *
     * \param[in] value Number of threads for transport.
     * \return This.
     */
    ExecutorConfig& num_transport_threads(std::size_t value);

    /*!
     * \brief Set the number of threads for callbacks.
     *
     * \param[in] value Number of threads for callbacks.
     * \return This.
     */
    ExecutorConfig& num_callback_threads(std::size_t value);

    /*!
     * \brief Get the number of threads for transport.
     *
     * \return Number of threads for transport.
     */
    [[nodiscard]] std::size_t num_transport_threads() const noexcept;

    /*!
     * \brief Get the number of threads for callbacks.
     *
     * \return Number of threads for callbacks.
     */
    [[nodiscard]] std::size_t num_callback_threads() const noexcept;

private:
    //! Number of threads for transport.
    std::size_t num_transport_threads_;

    //! Number of threads for callbacks.
    std::size_t num_callback_threads_;
};

}  // namespace msgpack_rpc::config
