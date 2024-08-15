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
 * \brief Definition of ShmStreamReader class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>
#include <cstdint>
#include <limits>

#include <boost/atomic/ipc_atomic.hpp>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/messages/buffer_view.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Class to read data to a stream on shared memory.
 */
class MSGPACK_RPC_EXPORT ShmStreamReader {
public:
    //! Type of indices.
    using Index = std::uint32_t;

    //! Type of atomic variables for indices.
    using AtomicIndex = boost::atomics::ipc_atomic<Index>;

    //! Maximum size of the buffer.
    static constexpr Index MAX_BUFFER_SIZE =
        std::numeric_limits<Index>::max() / 2U;

    //! Minimum size of the buffer.
    static constexpr Index MIN_BUFFER_SIZE = 2U;

    /*!
     * \brief Constructor.
     *
     * \param[in] atomic_next_written_index Atomic variable of the index of the
     * next byte to write.
     * \param[in] atomic_next_read_index Atomic variable of the index of the
     * next byte to read.
     * \param[in] buffer Buffer.
     */
    ShmStreamReader(AtomicIndex* atomic_next_written_index,
        AtomicIndex* atomic_next_read_index, messages::BufferView buffer);

    ShmStreamReader(const ShmStreamReader&) = delete;
    ShmStreamReader(ShmStreamReader&&) = delete;
    ShmStreamReader& operator=(const ShmStreamReader&) = delete;
    ShmStreamReader& operator=(ShmStreamReader&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~ShmStreamReader() = default;

    /*!
     * \brief Read data as much as possible.
     *
     * \param[out] data Buffer to write data.
     * \param[in] data_size Number of bytes of the buffer.
     * \return Number of bytes read.
     *
     * \note This function returns immediately. When no byte can be read,
     * this function returns zero.
     * \note This function is wait-free on most environments.
     */
    [[nodiscard]] std::size_t read_some(
        char* data, std::size_t data_size) noexcept;

private:
    /*!
     * \brief Calculate the number of continuous bytes readable currently.
     *
     * \param[in] next_written_index Value loaded from
     * atomic_next_written_index_.
     * \return Number of bytes.
     */
    [[nodiscard]] Index calc_continuously_readable_size(
        Index next_written_index) const noexcept;

    //! Atomic variable of the index of the next byte to write.
    AtomicIndex* atomic_next_written_index_;

    //! Atomic variable of the index of the next byte to read.
    AtomicIndex* atomic_next_read_index_;

    //! Pointer to the buffer.
    char* buffer_;

    //! Size of the buffer.
    Index buffer_size_;

    //! Index of the next byte to read.
    Index next_read_index_;
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
