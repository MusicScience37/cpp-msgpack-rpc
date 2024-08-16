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
 * \brief Implementation of ClientMemoryAddressCalculator class.
 */
#include "msgpack_rpc/transport/posix_shm/client_memory_address_calculator.h"

#include "msgpack_rpc/config.h"
#include "msgpack_rpc/messages/buffer_view.h"
#include "msgpack_rpc/transport/posix_shm/shm_stream_writer.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>
#include <cstdint>

#include <boost/atomic/ipc_atomic.hpp>

#include "msgpack_rpc/transport/posix_shm/memory_address_utils.h"

namespace msgpack_rpc::transport::posix_shm {

ClientMemoryAddressCalculator::ClientMemoryAddressCalculator(
    void* starting_address) noexcept
    : starting_address_(starting_address),
      parameters_(
          static_cast<const ClientMemoryParameters*>(starting_address)) {}

const ClientMemoryParameters* ClientMemoryAddressCalculator::parameters()
    const noexcept {
    return parameters_;
}

AtomicChangesCount* ClientMemoryAddressCalculator::changes_count()
    const noexcept {
    return static_cast<AtomicChangesCount*>(
        at(parameters_->changes_count_address));
}

AtomicClientState* ClientMemoryAddressCalculator::client_state()
    const noexcept {
    return static_cast<AtomicClientState*>(
        at(parameters_->client_state_address));
}

ShmStreamWriter ClientMemoryAddressCalculator::client_to_server_stream_writer()
    const noexcept {
    auto [atomic_next_written_index, atomic_next_read_index, buffer] =
        stream_at(parameters_->client_to_server_stream_address);
    return ShmStreamWriter{
        atomic_next_written_index, atomic_next_read_index, buffer};
}

ShmStreamReader ClientMemoryAddressCalculator::client_to_server_stream_reader()
    const noexcept {
    auto [atomic_next_written_index, atomic_next_read_index, buffer] =
        stream_at(parameters_->client_to_server_stream_address);
    return ShmStreamReader{
        atomic_next_written_index, atomic_next_read_index, buffer};
}

ShmStreamWriter ClientMemoryAddressCalculator::server_to_client_stream_writer()
    const noexcept {
    auto [atomic_next_written_index, atomic_next_read_index, buffer] =
        stream_at(parameters_->server_to_client_stream_address);
    return ShmStreamWriter{
        atomic_next_written_index, atomic_next_read_index, buffer};
}

ShmStreamReader ClientMemoryAddressCalculator::server_to_client_stream_reader()
    const noexcept {
    auto [atomic_next_written_index, atomic_next_read_index, buffer] =
        stream_at(parameters_->server_to_client_stream_address);
    return ShmStreamReader{
        atomic_next_written_index, atomic_next_read_index, buffer};
}

ClientMemoryParameters ClientMemoryAddressCalculator::calculate_parameters(
    std::size_t stream_buffer_size) {
    constexpr std::size_t parameters_size = sizeof(ClientMemoryParameters);
    constexpr std::size_t atomic_integer_size =
        sizeof(boost::atomics::ipc_atomic<std::uint32_t>);
    const std::size_t stream_size =
        atomic_integer_size * 2U + stream_buffer_size;

    constexpr std::size_t changes_count_address =
        calc_next_object_address(parameters_size, CACHE_LINE_ALIGNMENT);
    constexpr std::size_t client_state_address = calc_next_object_address(
        changes_count_address + atomic_integer_size, CACHE_LINE_ALIGNMENT);
    constexpr std::size_t client_to_server_stream_address =
        calc_next_object_address(
            client_state_address + atomic_integer_size, CACHE_LINE_ALIGNMENT);
    const std::size_t server_to_client_stream_address =
        calc_next_object_address(client_to_server_stream_address + stream_size,
            CACHE_LINE_ALIGNMENT);
    const std::size_t total_memory_size =
        server_to_client_stream_address + stream_size;

    ClientMemoryParameters parameters{};
    parameters.changes_count_address = changes_count_address;
    parameters.client_state_address = client_state_address;
    parameters.client_to_server_stream_address =
        client_to_server_stream_address;
    parameters.server_to_client_stream_address =
        server_to_client_stream_address;
    parameters.stream_buffer_size = stream_buffer_size;
    parameters.total_memory_size = total_memory_size;

    return parameters;
}

void* ClientMemoryAddressCalculator::at(
    std::size_t relative_address) const noexcept {
    return static_cast<void*>(
        static_cast<char*>(starting_address_) + relative_address);
}

std::tuple<ShmStreamWriter::AtomicIndex*, ShmStreamWriter::AtomicIndex*,
    messages::BufferView>
ClientMemoryAddressCalculator::stream_at(
    std::size_t relative_address) const noexcept {
    constexpr std::size_t atomic_integer_size =
        sizeof(boost::atomics::ipc_atomic<std::uint32_t>);

    auto* atomic_next_written_index =
        static_cast<ShmStreamWriter::AtomicIndex*>(at(relative_address));
    auto* atomic_next_read_index = static_cast<ShmStreamWriter::AtomicIndex*>(
        at(relative_address + atomic_integer_size));
    auto* buffer_data =
        static_cast<char*>(at(relative_address + atomic_integer_size * 2U));
    const std::size_t buffer_size = parameters_->stream_buffer_size;

    return {atomic_next_written_index, atomic_next_read_index,
        messages::BufferView{buffer_data, buffer_size}};
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
