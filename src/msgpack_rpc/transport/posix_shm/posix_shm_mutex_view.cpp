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
 * \brief Implementation of PosixShmMutexView class.
 */
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cassert>
#include <cerrno>
#include <string>

#include <fmt/format.h>
#include <pthread.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/transport/posix_shm/get_errno_message.h"

namespace msgpack_rpc::transport::posix_shm {

PosixShmMutexView::PosixShmMutexView(ActualMutex* mutex) noexcept
    : mutex_(mutex) {
    assert(mutex_ != nullptr);
}

void PosixShmMutexView::initialize() {
    pthread_mutexattr_t attributes;
    pthread_mutexattr_init(&attributes);

#ifndef NDEBUG
    // debug
    pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_ERRORCHECK);
#else
    // release
    pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_NORMAL);
#endif

    // Configuration for use on shared memory.
    if (const int errno_val =
            pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);
        errno_val > 0) {
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to configure an internal mutex for use on "
                        "shared memory: {}",
                get_errno_message(errno_val)));
    }

    // Configuration to prevent errors when a client process dies.
    pthread_mutexattr_setrobust(&attributes, PTHREAD_MUTEX_ROBUST);

    // Configuration to prevent priority inversion. (Optional.)
    pthread_mutexattr_setprotocol(&attributes, PTHREAD_PRIO_INHERIT);

    pthread_mutex_init(mutex_, &attributes);

    pthread_mutexattr_destroy(&attributes);
}

void PosixShmMutexView::lock() {
    const int errno_val = pthread_mutex_lock(mutex_);
    if (errno_val == 0) {
        return;
    }
    if (errno_val == EOWNERDEAD) {
        pthread_mutex_consistent(mutex_);
        return;
    }
    throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
        fmt::format("Failed to lock an internal mutex: {}",
            get_errno_message(errno_val)));
}

bool PosixShmMutexView::try_lock() {
    const int errno_val = pthread_mutex_trylock(mutex_);
    if (errno_val == 0) {
        return true;
    }
    if (errno_val == EBUSY) {
        // Another thread is locking this mutex.
        return false;
    }
    if (errno_val == EOWNERDEAD) {
        pthread_mutex_consistent(mutex_);
        return true;
    }
    throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
        fmt::format("Failed to lock an internal mutex: {}",
            get_errno_message(errno_val)));
}

void PosixShmMutexView::unlock() {
    const int errno_val = pthread_mutex_unlock(mutex_);
    if (errno_val == 0) {
        return;
    }
    throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
        fmt::format("Failed to unlock an internal mutex: {}",
            get_errno_message(errno_val)));
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
