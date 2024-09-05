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
 * \brief Implementation of PosixShmConditionVariableView class.
 */
#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cassert>
#include <cerrno>
#include <ctime>
#include <string>

#include <fmt/format.h>
#include <pthread.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/transport/posix_shm/get_errno_message.h"

namespace msgpack_rpc::transport::posix_shm {

PosixShmConditionVariableView::PosixShmConditionVariableView(
    ActualConditionVariable* condition_variable) noexcept
    : condition_variable_(condition_variable) {
    assert(condition_variable_ != nullptr);
}

void PosixShmConditionVariableView::initialize() {
    pthread_condattr_t attributes{};
    pthread_condattr_init(&attributes);

    // Configuration for use on shared memory.
    if (const int errno_val =
            pthread_condattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);
        errno_val > 0) {
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to configure an internal condition variable "
                        "for use on shared memory: {}",
                get_errno_message(errno_val)));
    }

    if (const int errno_val = pthread_condattr_setclock(&attributes, CLOCK_ID);
        errno_val > 0) {
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to configure an internal condition variable "
                        "to use a clock: {}",
                get_errno_message(errno_val)));
    }

    if (const int errno_val =
            pthread_cond_init(condition_variable_, &attributes);
        errno_val > 0) {
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format(
                "Failed to initialize an internal condition variable: {}",
                get_errno_message(errno_val)));
    }

    pthread_condattr_destroy(&attributes);
}

void PosixShmConditionVariableView::notify_one() {
    const int errno_val = pthread_cond_signal(condition_variable_);
    if (errno_val == 0) {
        return;
    }
    throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
        fmt::format("Failed to signal an internal condition variable: {}",
            get_errno_message(errno_val)));
}

void PosixShmConditionVariableView::notify_all() {
    const int errno_val = pthread_cond_broadcast(condition_variable_);
    if (errno_val == 0) {
        return;
    }
    throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
        fmt::format("Failed to signal an internal condition variable: {}",
            get_errno_message(errno_val)));
}

std::timespec PosixShmConditionVariableView::to_absolute_timeout(
    std::chrono::nanoseconds relative_timeout) {
    // NOLINTNEXTLINE(google-runtime-int): Type of std::timespec::tv_nsec defined by C++ standard.
    using NanoSecType = long;
    constexpr NanoSecType second_to_nanosecond = 1000000000;

    std::timespec absolute_timeout{};
    clock_gettime(CLOCK_ID, &absolute_timeout);

    if (relative_timeout.count() <= 0) {
        // Negative numbers can cause undefined behaviors in divisions.
        return absolute_timeout;
    }

    // Add the relative timeout preventing overflow of tv_nsec variable.
    absolute_timeout.tv_sec += static_cast<std::time_t>(
        relative_timeout.count() / second_to_nanosecond);
    absolute_timeout.tv_nsec += static_cast<NanoSecType>(
        relative_timeout.count() % second_to_nanosecond);

    // Carry forward.
    absolute_timeout.tv_sec += static_cast<std::time_t>(
        absolute_timeout.tv_nsec / second_to_nanosecond);
    absolute_timeout.tv_nsec %= second_to_nanosecond;

    return absolute_timeout;
}

void PosixShmConditionVariableView::wait(
    std::unique_lock<PosixShmMutexView>& lock) {
    const int errno_val =
        pthread_cond_wait(condition_variable_, lock.mutex()->actual_mutex());
    if (errno_val == 0) {
        return;
    }
    throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
        fmt::format("Failed to wait an internal condition variable: {}",
            get_errno_message(errno_val)));
}

bool PosixShmConditionVariableView::wait_until(
    std::unique_lock<PosixShmMutexView>& lock, const std::timespec& timeout) {
    const int errno_val = pthread_cond_timedwait(
        condition_variable_, lock.mutex()->actual_mutex(), &timeout);
    if (errno_val == 0) {
        return true;
    }
    if (errno_val == ETIMEDOUT) {
        return false;
    }
    throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
        fmt::format("Failed to wait an internal condition variable: {}",
            get_errno_message(errno_val)));
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
