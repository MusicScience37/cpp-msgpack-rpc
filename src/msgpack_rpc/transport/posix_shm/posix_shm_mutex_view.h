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
 * \brief Definition of PosixShmMutexView class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <pthread.h>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Class to handle mutexes on shared memory in POSIX.
 *
 * \note This class does not manage the life time of mutexes.
 */
class MSGPACK_RPC_EXPORT PosixShmMutexView {
public:
    //! Type of actual mutexes.
    using ActualMutex = pthread_mutex_t;

    /*!
     * \brief Constructor.
     *
     * \param[in] mutex Pointer to the actual mutex.
     *
     * \note This won't initialize the mutex. When initialization is required,
     * call initialize() function.
     */
    explicit PosixShmMutexView(ActualMutex* mutex) noexcept;

    /*!
     * \brief Initialize this mutex.
     *
     * \note This configures the mutex for use on shared memory.
     */
    void initialize();

    /*!
     * \brief Lock this mutex.
     */
    void lock();

    /*!
     * \brief Try to lock this mutex.
     *
     * \retval true Succeeded.
     * \retval false Failed because another thread is locking this mutex. (Throw
     * exceptions for other errors.)
     */
    [[nodiscard]] bool try_lock();

    /*!
     * \brief Unlock this mutex.
     *
     * \note This function may throw an exception when this mutex is not
     * correctly used.
     */
    void unlock();

    /*!
     * \brief Get the pointer to the actual mutex.
     *
     * \return Pointer to the actual mutex.
     */
    [[nodiscard]] ActualMutex* actual_mutex() const noexcept;

private:
    //! Pointer to the actual mutex.
    ActualMutex* mutex_;
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
