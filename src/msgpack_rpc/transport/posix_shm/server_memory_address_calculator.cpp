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
 * \brief Implementation of ServerMemoryAddressCalculator class.
 */
#include "msgpack_rpc/transport/posix_shm/server_memory_address_calculator.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>

#include "msgpack_rpc/transport/posix_shm/memory_address_utils.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"
#include "msgpack_rpc/transport/posix_shm/server_event_queue.h"
#include "msgpack_rpc/transport/posix_shm/server_state.h"

namespace msgpack_rpc::transport::posix_shm {

ServerMemoryAddressCalculator::ServerMemoryAddressCalculator(
    void* starting_address)
    : starting_address_(starting_address),
      parameters_(static_cast<ServerMemoryParameters*>(starting_address)) {}

const ServerMemoryParameters* ServerMemoryAddressCalculator::parameters()
    const noexcept {
    return parameters_;
}

AtomicServerState* ServerMemoryAddressCalculator::server_state()
    const noexcept {
    return static_cast<AtomicServerState*>(
        at(parameters_->server_state_address));
}

ServerEventQueue ServerMemoryAddressCalculator::server_event_queue()
    const noexcept {
    std::size_t relative_address = parameters_->event_queue_address;
    PosixShmMutexView mutex(
        static_cast<PosixShmMutexView::ActualMutex*>(at(relative_address)));

    constexpr std::size_t mutex_size = sizeof(PosixShmMutexView::ActualMutex);
    constexpr std::size_t mutex_size_with_padding =
        calc_next_object_address(mutex_size, CACHE_LINE_ALIGNMENT);
    relative_address += mutex_size_with_padding;
    PosixShmConditionVariableView condition_variable(
        static_cast<PosixShmConditionVariableView::ActualConditionVariable*>(
            at(relative_address)));

    constexpr std::size_t condition_variable_size =
        sizeof(PosixShmConditionVariableView::ActualConditionVariable);
    constexpr std::size_t condition_variable_size_with_padding =
        calc_next_object_address(condition_variable_size, CACHE_LINE_ALIGNMENT);
    relative_address += sizeof(condition_variable_size_with_padding);
    auto* next_written_index =
        static_cast<ServerEventQueue::Index*>(at(relative_address));

    relative_address += sizeof(ServerEventQueue::Index);
    auto* next_read_index =
        static_cast<ServerEventQueue::Index*>(at(relative_address));

    relative_address += sizeof(ServerEventQueue::Index);
    auto* buffer = static_cast<ServerEvent*>(at(relative_address));

    return ServerEventQueue{mutex, condition_variable, next_written_index,
        next_read_index, buffer, parameters_->event_queue_buffer_size};
}

ServerMemoryParameters ServerMemoryAddressCalculator::calculate_parameters(
    std::size_t event_queue_buffer_size) {
    constexpr std::size_t parameters_size = sizeof(ServerMemoryParameters);
    constexpr std::size_t atomic_integer_size =
        sizeof(boost::atomics::ipc_atomic<std::uint32_t>);
    constexpr std::size_t integer_size = sizeof(std::uint32_t);
    constexpr std::size_t mutex_size = sizeof(PosixShmMutexView::ActualMutex);
    constexpr std::size_t mutex_size_with_padding =
        calc_next_object_address(mutex_size, CACHE_LINE_ALIGNMENT);
    constexpr std::size_t condition_variable_size =
        sizeof(PosixShmConditionVariableView::ActualConditionVariable);
    constexpr std::size_t condition_variable_size_with_padding =
        calc_next_object_address(condition_variable_size, CACHE_LINE_ALIGNMENT);

    constexpr std::size_t server_state_address =
        calc_next_object_address(parameters_size, CACHE_LINE_ALIGNMENT);
    constexpr std::size_t event_queue_address = calc_next_object_address(
        server_state_address + atomic_integer_size, CACHE_LINE_ALIGNMENT);
    const std::size_t total_memory_size = event_queue_address +
        mutex_size_with_padding + condition_variable_size_with_padding +
        2 * integer_size + event_queue_buffer_size * sizeof(ServerEvent);

    ServerMemoryParameters parameters{};
    parameters.server_state_address = server_state_address;
    parameters.event_queue_address = event_queue_address;
    parameters.event_queue_buffer_size = event_queue_buffer_size;
    parameters.total_memory_size = total_memory_size;

    return parameters;
}

void* ServerMemoryAddressCalculator::at(
    std::size_t relative_address) const noexcept {
    return static_cast<void*>(
        static_cast<char*>(starting_address_) + relative_address);
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
