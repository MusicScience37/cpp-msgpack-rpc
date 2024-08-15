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
 * \brief Definition of PosixShmConditionVariableView class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <chrono>
#include <ctime>
#include <mutex>
#include <utility>

#include <pthread.h>
#include <sys/types.h>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Class to handle condition variables on shared memory in POSIX.
 *
 * \note This class does not manage the life time of condition variables.
 */
class MSGPACK_RPC_EXPORT PosixShmConditionVariableView {
public:
    //! Type of actual condition variables.
    using ActualConditionVariable = pthread_cond_t;

    //! ID of the clock used in
    static constexpr clockid_t CLOCK_ID = CLOCK_MONOTONIC;

    /*!
     * \brief Constructor.
     *
     * \param[in] condition_variable Pointer to the actual condition variable.
     *
     * \note This won't initialize the condition variable. When initialization
     * is required, call initialize() function.
     */
    explicit PosixShmConditionVariableView(
        ActualConditionVariable* condition_variable) noexcept;

    /*!
     * \brief Initialize this condition variable.
     *
     * \note This configures the condition variable for use on shared memory.
     */
    void initialize();

    /*!
     * \brief Wait the condition variable.
     *
     * \tparam Condition Type of the function of the condition.
     * \param[in] lock Lock.
     * \param[in] condition Function of the condition.
     *
     * \note This function works as `std::condition_variable::wait` function.
     */
    template <typename Condition>
    void wait(std::unique_lock<PosixShmMutexView>& lock, Condition condition) {
        while (!condition()) {
            wait(lock);
        }
    }

    /*!
     * \brief Wait the condition variable.
     *
     * \tparam Condition Type of the function of the condition.
     * \param[in] lock Lock.
     * \param[in] timeout Timeout.
     * \param[in] condition Function of the condition.
     * \retval Result of `condition()` at the end of waiting.
     *
     * \note This function works as `std::condition_variable::wait_until`
     * function.
     */
    template <typename Condition>
    bool wait_until(std::unique_lock<PosixShmMutexView>& lock,
        const std::timespec& timeout, Condition condition) {
        while (!condition()) {
            if (!wait_until(lock, timeout)) {
                return condition();
            }
        }
        return true;
    }

    /*!
     * \brief Wait the condition variable.
     *
     * \tparam Condition Type of the function of the condition.
     * \param[in] lock Lock.
     * \param[in] timeout Timeout.
     * \param[in] condition Function of the condition.
     * \retval Result of `condition()` at the end of waiting.
     *
     * \note This function works as `std::condition_variable::wait_for`
     * function.
     */
    template <typename Condition>
    bool wait_for(std::unique_lock<PosixShmMutexView>& lock,
        std::chrono::nanoseconds timeout, Condition&& condition) {
        return wait_until(lock, to_absolute_timeout(timeout),
            std::forward<Condition>(condition));
    }

    /*!
     * \brief Notify a thread to wake.
     *
     * \note This function works as `std::condition_variable::notify_one`
     * function.
     */
    void notify_one();

    /*!
     * \brief Notify all threads to wake.
     *
     * \note This function works as `std::condition_variable::notify_all`
     * function.
     */
    void notify_all();

    /*!
     * \brief Convert a relative timeout to an absolute timeout.
     *
     * \param[in] relative_timeout Relative timeout.
     * \return Absolute timeout.
     */
    static std::timespec to_absolute_timeout(
        std::chrono::nanoseconds relative_timeout);

private:
    /*!
     * \brief Wait the condition variable.
     *
     * \param[in] lock Lock.
     */
    void wait(std::unique_lock<PosixShmMutexView>& lock);

    /*!
     * \brief Wait the condition variable for the given time.
     *
     * \param[in] lock Lock.
     * \param[in] timeout Timeout.
     * \retval true Finished waiting before timeout.
     * \retval false Finished waiting due to timeout.
     */
    bool wait_until(std::unique_lock<PosixShmMutexView>& lock,
        const std::timespec& timeout);

    //! Pointer to the actual condition variables.
    ActualConditionVariable* condition_variable_;
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
