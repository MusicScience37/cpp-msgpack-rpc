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

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::config {

namespace {

constexpr auto RECONNECTION_CONFIG_DEFAULT_INITIAL_WAITING_TIME =
    std::chrono::milliseconds(125);

constexpr auto RECONNECTION_CONFIG_DEFAULT_MAX_WAITING_TIME =
    std::chrono::seconds(32);

constexpr auto RECONNECTION_CONFIG_DEFAULT_MAX_JITTER_WAITING_TIME =
    RECONNECTION_CONFIG_DEFAULT_INITIAL_WAITING_TIME;

}  // namespace

ReconnectionConfig::ReconnectionConfig()
    : initial_waiting_time_(RECONNECTION_CONFIG_DEFAULT_INITIAL_WAITING_TIME),
      max_waiting_time_(RECONNECTION_CONFIG_DEFAULT_MAX_WAITING_TIME),
      max_jitter_waiting_time_(
          RECONNECTION_CONFIG_DEFAULT_MAX_JITTER_WAITING_TIME) {}

ReconnectionConfig& ReconnectionConfig::initial_waiting_time(
    std::chrono::nanoseconds value) {
    if (value <= std::chrono::nanoseconds(0)) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Waiting time must be larger than zero.");
    }
    initial_waiting_time_ = value;
    return *this;
}

std::chrono::nanoseconds ReconnectionConfig::initial_waiting_time()
    const noexcept {
    return initial_waiting_time_;
}

ReconnectionConfig& ReconnectionConfig::max_waiting_time(
    std::chrono::nanoseconds value) {
    if (value <= std::chrono::nanoseconds(0)) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Waiting time must be larger than zero.");
    }
    max_waiting_time_ = value;
    return *this;
}

std::chrono::nanoseconds ReconnectionConfig::max_waiting_time() const noexcept {
    return max_waiting_time_;
}

ReconnectionConfig& ReconnectionConfig::max_jitter_waiting_time(
    std::chrono::nanoseconds value) {
    if (value < std::chrono::nanoseconds(0)) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Jitter must be larger than or equal to zero.");
    }
    max_jitter_waiting_time_ = value;
    return *this;
}

std::chrono::nanoseconds ReconnectionConfig::max_jitter_waiting_time()
    const noexcept {
    return max_jitter_waiting_time_;
}

}  // namespace msgpack_rpc::config
