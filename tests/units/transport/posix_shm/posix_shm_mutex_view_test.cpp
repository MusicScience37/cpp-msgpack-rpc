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
 * \brief Test of PosixShmMutexView class.
 */
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <pthread.h>

TEST_CASE("msgpack_rpc::transport::posix_shm::PosixShmMutexView") {
    using msgpack_rpc::transport::posix_shm::PosixShmMutexView;

    SECTION("lock and unlock") {
        PosixShmMutexView::ActualMutex mutex{};
        PosixShmMutexView view{&mutex};
        REQUIRE_NOTHROW(view.initialize());

        REQUIRE_NOTHROW(view.lock());
        REQUIRE_NOTHROW(view.unlock());
    }

    SECTION("try to lock") {
        PosixShmMutexView::ActualMutex mutex{};
        PosixShmMutexView view{&mutex};
        REQUIRE_NOTHROW(view.initialize());

        REQUIRE(view.try_lock());
        REQUIRE_NOTHROW(view.unlock());
    }

    SECTION("try to lock when another thread is holding a lock") {
        PosixShmMutexView::ActualMutex mutex{};
        PosixShmMutexView view{&mutex};
        REQUIRE_NOTHROW(view.initialize());

        REQUIRE(view.try_lock());

        bool result = true;
        std::thread{[&mutex, &result] {
            PosixShmMutexView view{&mutex};
            result = view.try_lock();
        }}.join();
        CHECK_FALSE(result);

        REQUIRE_NOTHROW(view.unlock());
    }

    SECTION("lock after a thread dies holding a lock") {
        PosixShmMutexView::ActualMutex mutex{};
        PosixShmMutexView view{&mutex};
        REQUIRE_NOTHROW(view.initialize());

        std::thread{[&mutex] {
            PosixShmMutexView view{&mutex};
            view.lock();
        }}.join();

        REQUIRE(view.try_lock());
        REQUIRE_NOTHROW(view.unlock());
        // Repeat locking to check that the lock is working.
        REQUIRE(view.try_lock());
        REQUIRE_NOTHROW(view.unlock());
        REQUIRE(view.try_lock());
        REQUIRE_NOTHROW(view.unlock());
    }

    SECTION("check errors in pthread APIs") {
        PosixShmMutexView::ActualMutex mutex{};
        PosixShmMutexView view{&mutex};
        REQUIRE_NOTHROW(view.initialize());
        pthread_mutex_destroy(&mutex);

        CHECK_THROWS(view.lock());
        CHECK_THROWS((void)view.try_lock());
        CHECK_THROWS(view.unlock());
    }
}

#endif
