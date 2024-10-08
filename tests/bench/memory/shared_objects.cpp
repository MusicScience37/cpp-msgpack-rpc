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
 * \brief Benchmark of memory management of shared objects.
 */
#include "msgpack_rpc/util/impl/shared_objects.h"

#include <memory>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/current_invocation_context.h>
#include <stat_bench/do_not_optimize.h>

struct Object {
    std::size_t num{0};
};

STAT_BENCH_CASE("shared_objects", "make_shared") {
    const std::size_t num_iterations =
        stat_bench::current_invocation_context().iterations();

    std::vector<std::shared_ptr<Object>> objects;
    objects.resize(num_iterations);
    STAT_BENCH_MEASURE_INDEXED(
        /*thread_index*/, /*sample_index*/, iteration_index) {
        objects[iteration_index] = std::make_shared<Object>();
    };
}

STAT_BENCH_CASE("shared_objects", "SharedObjectMemoryPool") {
    const std::size_t num_iterations =
        stat_bench::current_invocation_context().iterations();

    const std::size_t num_buffers = num_iterations + 1U;
    const auto memory_pool =
        msgpack_rpc::util::impl::SharedObjectMemoryPool<Object>::create(
            num_buffers);

    std::vector<msgpack_rpc::util::impl::SharedObject<Object>> objects;
    objects.resize(num_iterations);
    STAT_BENCH_MEASURE_INDEXED(
        /*thread_index*/, /*sample_index*/, iteration_index) {
        objects[iteration_index] = memory_pool->create();
    };
}

STAT_BENCH_MAIN
