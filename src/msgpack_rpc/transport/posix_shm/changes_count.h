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
 * \brief Definition of ChangesCount class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <chrono>
#include <cstdint>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Class to count changes to shared memory.
 */
class MSGPACK_RPC_EXPORT ChangesCount {
public:
    //! Type of the count.
    using Count = std::uint32_t;

    /*!
     * \brief Constructor.
     *
     * \param[in] mutex Mutex.
     * \param[in] condition_variable Condition variable.
     * \param[in] count Count.
     */
    ChangesCount(PosixShmMutexView mutex,
        PosixShmConditionVariableView condition_variable, Count* count);

    /*!
     * \brief Initialize this counter.
     */
    void initialize();

    /*!
     * \brief Increment the count.
     *
     * \note This function notifies threads waiting using wait_for_change_from()
     * function.
     */
    void increment();

    /*!
     * \brief Get the current count.
     *
     * \return Count.
     */
    [[nodiscard]] Count count();

    /*!
     * \brief Wait for change from the given count.
     *
     * \param[in] count Count.
     * \param[in] timeout Timeout.
     * \return Whether the count is changed.
     */
    bool wait_for_change_from(Count count, std::chrono::nanoseconds timeout);

private:
    //! Mutex.
    PosixShmMutexView mutex_;

    //! Condition variable.
    PosixShmConditionVariableView condition_variable_;

    //! Count.
    Count* count_;
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
