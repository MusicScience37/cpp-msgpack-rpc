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
 * \brief Implementation of ChangesCount class.
 */
#include "msgpack_rpc/transport/posix_shm/changes_count.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <mutex>

#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

namespace msgpack_rpc::transport::posix_shm {

ChangesCount::ChangesCount(PosixShmMutexView mutex,
    PosixShmConditionVariableView condition_variable, Count* count)
    : mutex_(mutex), condition_variable_(condition_variable), count_(count) {}

void ChangesCount::initialize() {
    mutex_.initialize();
    condition_variable_.initialize();
    *count_ = 0;
}

void ChangesCount::increment() {
    std::unique_lock<PosixShmMutexView> lock(mutex_);
    ++(*count_);
    lock.unlock();
    condition_variable_.notify_all();
}

ChangesCount::Count ChangesCount::count() {
    std::unique_lock<PosixShmMutexView> lock(mutex_);
    return *count_;
}

bool ChangesCount::wait_for_change_from(
    Count count, std::chrono::nanoseconds timeout) {
    std::unique_lock<PosixShmMutexView> lock(mutex_);
    return condition_variable_.wait_for(
        lock, timeout, [this, &count] { return *count_ != count; });
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
