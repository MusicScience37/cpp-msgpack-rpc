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

namespace msgpack_rpc::config {

namespace {

constexpr auto RECONNECTION_CONFIG_DEFAULT_WAIT_TIME =
    std::chrono::milliseconds(100);

}

ReconnectionConfig::ReconnectionConfig()
    : wait_time_(RECONNECTION_CONFIG_DEFAULT_WAIT_TIME) {}

ReconnectionConfig& ReconnectionConfig::wait_time(
    std::chrono::nanoseconds value) {
    wait_time_ = value;
    return *this;
}

std::chrono::nanoseconds ReconnectionConfig::wait_time() const noexcept {
    return wait_time_;
}

}  // namespace msgpack_rpc::config
