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
 * \brief Implementation of ExecutorConfig class.
 */
#include "msgpack_rpc/config/executor_config.h"

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::config {

ExecutorConfig::ExecutorConfig()
    : num_transport_threads_(1), num_callback_threads_(1) {}

ExecutorConfig& ExecutorConfig::num_transport_threads(std::size_t value) {
    if (value <= 0U) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Invalid number of transport threads.");
    }
    num_transport_threads_ = value;
    return *this;
}

ExecutorConfig& ExecutorConfig::num_callback_threads(std::size_t value) {
    if (value <= 0U) {
        throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
            "Invalid number of callback threads.");
    }
    num_callback_threads_ = value;
    return *this;
}

std::size_t ExecutorConfig::num_transport_threads() const noexcept {
    return num_transport_threads_;
}

std::size_t ExecutorConfig::num_callback_threads() const noexcept {
    return num_callback_threads_;
}

}  // namespace msgpack_rpc::config
