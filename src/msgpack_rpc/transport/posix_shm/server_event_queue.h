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
 * \brief Definition of ServerEventQueue class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

#include <boost/atomic/ipc_atomic.hpp>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/transport/posix_shm/client_id.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.h"
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Enumeration of types of events in servers.
 */
enum class ServerEventType : std::uint32_t {
    //! A client is created.
    CLIENT_CREATED = 1,

    //! Some changes occurred in a client.
    CLIENT_STATE_CHANGED = 2,

    //! A client is destroyed.
    CLIENT_DESTROYED = 3
};

/*!
 * \brief Struct of events in servers.
 */
struct ServerEvent {
    //! ID of the client.
    ClientID client_id;

    //! Type of the event.
    ServerEventType type;
};

/*!
 * \brief Class of queues of events in servers.
 *
 * - Events can be pushed from multiple threads in clients.
 * - Events can be popped from a single thread in a server.
 */
class MSGPACK_RPC_EXPORT ServerEventQueue {
public:
    //! Type of indices.
    using Index = std::uint32_t;

    //! Maximum size of the buffer.
    static constexpr Index MAX_BUFFER_SIZE =
        std::numeric_limits<Index>::max() / 2U;

    //! Minimum size of the buffer.
    static constexpr Index MIN_BUFFER_SIZE = 2U;

    /*!
     * \brief Constructor.
     *
     * \param[in] mutex Mutex.
     * \param[in] condition_variable Condition variable.
     * \param[in] next_written_index Index of the next element to write.
     * \param[in] next_read_index Index of the next element to read.
     * \param[in] buffer Buffer.
     * \param[in] buffer_size Size of the buffer.
     */
    ServerEventQueue(PosixShmMutexView mutex,
        PosixShmConditionVariableView condition_variable,
        Index* next_written_index, Index* next_read_index, ServerEvent* buffer,
        std::size_t buffer_size);

    /*!
     * \brief Initialize this queue.
     */
    void initialize();

    /*!
     * \brief Push an event.
     *
     * \param[in] event Event.
     * \param[in] timeout Timeout.
     * \retval true Succeeded.
     * \retval false Failed because the buffer is full.
     */
    [[nodiscard]] bool push(
        const ServerEvent& event, std::chrono::nanoseconds timeout) noexcept;

    /*!
     * \brief Pop an event.
     *
     * \param[in] timeout Timeout.
     * \return Popped event. Null if the buffer is empty.
     */
    [[nodiscard]] std::optional<ServerEvent> pop(
        std::chrono::nanoseconds timeout) noexcept;

private:
    //! Mutex.
    PosixShmMutexView mutex_;

    //! Condition variable.
    PosixShmConditionVariableView condition_variable_;

    //! Index of the next element to write.
    Index* next_written_index_;

    //! Index of the next element to read.
    Index* next_read_index_;

    //! Buffer.
    ServerEvent* buffer_;

    //! Size of the buffer.
    Index buffer_size_;
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
