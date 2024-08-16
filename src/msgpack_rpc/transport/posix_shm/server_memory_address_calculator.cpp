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

#include "msgpack_rpc/transport/posix_shm/changes_count.h"
#include "msgpack_rpc/transport/posix_shm/memory_address_utils.h"
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

AtomicChangesCount* ServerMemoryAddressCalculator::changes_count()
    const noexcept {
    return static_cast<AtomicChangesCount*>(
        at(parameters_->changes_count_address));
}

AtomicServerState* ServerMemoryAddressCalculator::server_state()
    const noexcept {
    return static_cast<AtomicServerState*>(
        at(parameters_->server_state_address));
}

ServerEventQueue ServerMemoryAddressCalculator::server_event_queue()
    const noexcept {
    constexpr std::size_t atomic_integer_size =
        sizeof(boost::atomics::ipc_atomic<std::uint32_t>);

    PosixShmMutexView writer_mutex(static_cast<PosixShmMutexView::ActualMutex*>(
        at(parameters_->event_queue_writer_mutex_address)));
    auto* atomic_next_written_index =
        static_cast<ServerEventQueue::AtomicIndex*>(
            at(parameters_->event_queue_rest_address));
    auto* atomic_next_read_index = static_cast<ServerEventQueue::AtomicIndex*>(
        at(parameters_->event_queue_rest_address + atomic_integer_size));
    auto* buffer = static_cast<ServerEvent*>(
        at(parameters_->event_queue_rest_address + 2 * atomic_integer_size));

    return ServerEventQueue{writer_mutex, atomic_next_written_index,
        atomic_next_read_index, buffer, parameters_->event_queue_buffer_size};
}

ServerMemoryParameters ServerMemoryAddressCalculator::calculate_parameters(
    std::size_t event_queue_buffer_size) {
    constexpr std::size_t parameters_size = sizeof(ServerMemoryParameters);
    constexpr std::size_t atomic_integer_size =
        sizeof(boost::atomics::ipc_atomic<std::uint32_t>);
    constexpr std::size_t mutex_size = sizeof(PosixShmMutexView::ActualMutex);

    constexpr std::size_t changes_count_address =
        calc_next_object_address(parameters_size, CACHE_LINE_ALIGNMENT);
    constexpr std::size_t server_state_address = calc_next_object_address(
        changes_count_address + atomic_integer_size, CACHE_LINE_ALIGNMENT);
    constexpr std::size_t event_queue_writer_mutex_address =
        calc_next_object_address(
            server_state_address + atomic_integer_size, CACHE_LINE_ALIGNMENT);
    constexpr std::size_t event_queue_rest_address = calc_next_object_address(
        event_queue_writer_mutex_address + mutex_size, CACHE_LINE_ALIGNMENT);
    const std::size_t total_memory_size = event_queue_rest_address +
        2 * atomic_integer_size + event_queue_buffer_size * sizeof(ServerEvent);

    ServerMemoryParameters parameters{};
    parameters.changes_count_address = changes_count_address;
    parameters.server_state_address = server_state_address;
    parameters.event_queue_writer_mutex_address =
        event_queue_writer_mutex_address;
    parameters.event_queue_rest_address = event_queue_rest_address;
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
