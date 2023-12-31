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
 * \brief Test of ExecutorConfig class.
 */
#include "msgpack_rpc/config/executor_config.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::config::ExecutorConfig") {
    using msgpack_rpc::config::ExecutorConfig;

    ExecutorConfig config;

    SECTION("set num_transport_threads") {
        SECTION("correct values") {
            CHECK(
                config.num_transport_threads(2U).num_transport_threads() == 2U);
            CHECK(
                config.num_transport_threads(1U).num_transport_threads() == 1U);
        }

        SECTION("invalid values") {
            CHECK_THROWS(config.num_transport_threads(0U));
        }
    }

    SECTION("set num_callback_threads") {
        SECTION("correct values") {
            CHECK(config.num_callback_threads(2U).num_callback_threads() == 2U);
            CHECK(config.num_callback_threads(1U).num_callback_threads() == 1U);
        }

        SECTION("invalid values") {
            CHECK_THROWS(config.num_callback_threads(0U));
        }
    }
}
