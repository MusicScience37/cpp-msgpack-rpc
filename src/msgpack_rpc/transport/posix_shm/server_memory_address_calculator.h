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
 * \brief Definition of ServerMemoryAddressCalculator class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/transport/posix_shm/changes_count.h"
#include "msgpack_rpc/transport/posix_shm/server_event_queue.h"
#include "msgpack_rpc/transport/posix_shm/server_state.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Struct of parameters of shared memory of servers.
 */
struct ServerMemoryParameters {
    //! Relative address of the count of changes.
    std::size_t changes_count_address;

    //! Relative address of the state of the server.
    std::size_t server_state_address;

    //! Relative address of the mutex of writers in the event queue.
    std::size_t event_queue_writer_mutex_address;

    //! Relative address of the remaining variables in the event queue.
    std::size_t event_queue_rest_address;

    //! Size of the buffer in the event queue.
    std::size_t event_queue_buffer_size;

    //! Total size of the shared memory.
    std::size_t total_memory_size;
};

/*!
 * \brief Class to calculate addresses on shared memory of servers.
 */
class MSGPACK_RPC_EXPORT ServerMemoryAddressCalculator {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] starting_address Starting address of the shared memory.
     */
    explicit ServerMemoryAddressCalculator(void* starting_address);

    /*!
     * \brief Get the parameters.
     *
     * \return Pointer to the parameters.
     */
    [[nodiscard]] const ServerMemoryParameters* parameters() const noexcept;

    /*!
     * \brief Get the count of changes.
     *
     * \return Pointer to the count of changes.
     */
    [[nodiscard]] AtomicChangesCount* changes_count() const noexcept;

    /*!
     * \brief Get the state of the server.
     *
     * \return Pointer to the state of the server.
     */
    [[nodiscard]] AtomicServerState* server_state() const noexcept;

    /*!
     * \brief Create an event queue of the server.
     *
     * \return Event queue.
     */
    [[nodiscard]] ServerEventQueue server_event_queue() const noexcept;

    /*!
     * \brief Calculate parameters.
     *
     * \param[in] event_queue_buffer_size Size of the buffer in the event queue.
     * \return Parameters.
     */
    [[nodiscard]] static ServerMemoryParameters calculate_parameters(
        std::size_t event_queue_buffer_size);

private:
    /*!
     * \brief Get the pointer of a relative address.
     *
     * \param[in] relative_address Relative address.
     * \return Pointer.
     */
    [[nodiscard]] void* at(std::size_t relative_address) const noexcept;

    //! Starting address of the shared memory.
    void* starting_address_;

    //! Parameters.
    const ServerMemoryParameters* parameters_;
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
