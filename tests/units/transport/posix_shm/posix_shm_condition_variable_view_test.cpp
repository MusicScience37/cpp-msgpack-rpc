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
 * \brief Test of PosixShmConditionVariableView class.
 */
#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <future>
#include <mutex>

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

TEST_CASE("msgpack_rpc::transport::posix_shm::PosixShmConditionVariableView") {
    using msgpack_rpc::transport::posix_shm::PosixShmConditionVariableView;
    using msgpack_rpc::transport::posix_shm::PosixShmMutexView;

    PosixShmMutexView::ActualMutex actual_mutex;
    PosixShmMutexView mutex{&actual_mutex};
    REQUIRE_NOTHROW(mutex.initialize());

    PosixShmConditionVariableView::ActualConditionVariable
        actual_condition_variable;
    PosixShmConditionVariableView condition_variable{
        &actual_condition_variable};
    REQUIRE_NOTHROW(condition_variable.initialize());

    SECTION("wait for a condition") {
        bool stop_waiting = false;
        auto future = std::async([&mutex, &condition_variable, &stop_waiting] {
            std::unique_lock<PosixShmMutexView> lock(mutex);
            condition_variable.wait(
                lock, [&stop_waiting] { return stop_waiting; });
        });

        {
            std::unique_lock<PosixShmMutexView> lock(mutex);
            stop_waiting = true;
            lock.unlock();
            condition_variable.notify_one();
        }

        const auto future_status = future.wait_for(std::chrono::seconds(1));
        CHECK(future_status == std::future_status::ready);
        if (future_status != std::future_status::ready) {
            // On this condition, this test won't finish correctly.
            std::abort();
        }
    }

    SECTION("wait for a condition with a timeout") {
        bool stop_waiting = false;
        auto future = std::async([&mutex, &condition_variable, &stop_waiting] {
            constexpr auto timeout = std::chrono::seconds(1);
            std::unique_lock<PosixShmMutexView> lock(mutex);
            return condition_variable.wait_for(
                lock, timeout, [&stop_waiting] { return stop_waiting; });
        });

        {
            std::unique_lock<PosixShmMutexView> lock(mutex);
            stop_waiting = true;
            lock.unlock();
            condition_variable.notify_all();
        }

        const auto future_status = future.wait_for(std::chrono::seconds(1));
        CHECK(future_status == std::future_status::ready);
        if (future_status != std::future_status::ready) {
            // On this condition, this test won't finish correctly.
            std::abort();
        }
        CHECK(future.get());
    }

    SECTION("wait until timeout") {
        bool stop_waiting = false;
        auto future = std::async([&mutex, &condition_variable, &stop_waiting] {
            constexpr auto timeout = std::chrono::milliseconds(10);
            std::unique_lock<PosixShmMutexView> lock(mutex);
            return condition_variable.wait_for(
                lock, timeout, [&stop_waiting] { return stop_waiting; });
        });

        // No one notify to the thread.

        const auto future_status = future.wait_for(std::chrono::seconds(1));
        CHECK(future_status == std::future_status::ready);
        if (future_status != std::future_status::ready) {
            // On this condition, this test won't finish correctly.
            std::abort();
        }
        CHECK_FALSE(future.get());
    }

    SECTION("convert timeout from a relative one to an absolute one") {
        constexpr std::chrono::nanoseconds relative_timeout =
            std::chrono::seconds(3) + std::chrono::nanoseconds(987654321);

        const std::timespec absolute_timeout =
            PosixShmConditionVariableView::to_absolute_timeout(
                relative_timeout);

        CHECK(absolute_timeout.tv_nsec >= 0);
        // NOLINTNEXTLINE(google-runtime-int): Type of std::timespec::tv_nsec defined by C++ standard.
        using NanoSecType = long;
        constexpr NanoSecType second_to_nanosecond = 1000000000;
        CHECK(absolute_timeout.tv_nsec < second_to_nanosecond);
    }
}

#endif
