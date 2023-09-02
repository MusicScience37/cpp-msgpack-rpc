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
 * \brief Implementation of ReconnectionConfig class.
 */
#include "msgpack_rpc/config/reconnection_config.h"

#include <chrono>
#include <ratio>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::config::ReconnectionConfig") {
    using msgpack_rpc::config::ReconnectionConfig;

    ReconnectionConfig config;

    SECTION("has correct value as default") {
        CHECK(config.initial_waiting_time() > std::chrono::nanoseconds(0));
        CHECK(config.max_waiting_time() > config.initial_waiting_time());
        CHECK(config.max_jitter_waiting_time() > std::chrono::nanoseconds(0));
    }

    SECTION("set the initial waiting time") {
        constexpr auto value = std::chrono::milliseconds(12345);

        config.initial_waiting_time(value);

        CHECK(config.initial_waiting_time() == value);
    }

    SECTION("set the maximum waiting time") {
        constexpr auto value = std::chrono::milliseconds(12345);

        config.max_waiting_time(value);

        CHECK(config.max_waiting_time() == value);
    }

    SECTION("set the maximum jitter of waiting time") {
        constexpr auto value = std::chrono::milliseconds(12345);

        config.max_jitter_waiting_time(value);

        CHECK(config.max_jitter_waiting_time() == value);
    }
}
