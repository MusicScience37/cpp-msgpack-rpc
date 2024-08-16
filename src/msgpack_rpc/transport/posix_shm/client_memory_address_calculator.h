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
 * \brief Definition of ClientMemoryAddressCalculator class.
 */
#pragma once

#include "msgpack_rpc/config.h"
#include "msgpack_rpc/transport/posix_shm/clint_state.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>
#include <tuple>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/messages/buffer_view.h"
#include "msgpack_rpc/transport/posix_shm/changes_count.h"
#include "msgpack_rpc/transport/posix_shm/shm_stream_reader.h"
#include "msgpack_rpc/transport/posix_shm/shm_stream_writer.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Struct of parameters of shared memory of clients.
 */
struct ClientMemoryParameters {
    //! Relative address of the count of changes.
    std::size_t changes_count_address;

    //! Relative address of the state of client.
    std::size_t client_state_address;

    //! Relative address of the stream from the client to the server.
    std::size_t client_to_server_stream_address;

    //! Relative address of the stream from the server to the client.
    std::size_t server_to_client_stream_address;

    //! Size of buffers of streams.
    std::size_t stream_buffer_size;

    //! Total size of the shared memory.
    std::size_t total_memory_size;
};

/*!
 * \brief Class to calculate addresses on shared memory of clients.
 */
class MSGPACK_RPC_EXPORT ClientMemoryAddressCalculator {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] starting_address Starting address of the shared memory.
     */
    explicit ClientMemoryAddressCalculator(void* starting_address) noexcept;

    /*!
     * \brief Get the parameters.
     *
     * \return Pointer to the parameters.
     */
    [[nodiscard]] const ClientMemoryParameters* parameters() const noexcept;

    /*!
     * \brief Get the count of changes.
     *
     * \return Pointer to the count of changes.
     */
    [[nodiscard]] AtomicChangesCount* changes_count() const noexcept;

    /*!
     * \brief Get the state of the client.
     *
     * \return Pointer to the state of the client.
     */
    [[nodiscard]] AtomicClientState* client_state() const noexcept;

    /*!
     * \brief Create a writer of the stream from the client to the server.
     *
     * \return Writer.
     */
    [[nodiscard]] ShmStreamWriter client_to_server_stream_writer()
        const noexcept;

    /*!
     * \brief Create a reader of the stream from the client to the server.
     *
     * \return Reader.
     */
    [[nodiscard]] ShmStreamReader client_to_server_stream_reader()
        const noexcept;

    /*!
     * \brief Create a writer of the stream from the server to the client.
     *
     * \return Writer.
     */
    [[nodiscard]] ShmStreamWriter server_to_client_stream_writer()
        const noexcept;

    /*!
     * \brief Create a reader of the stream from the server to the client.
     *
     * \return Reader.
     */
    [[nodiscard]] ShmStreamReader server_to_client_stream_reader()
        const noexcept;

    /*!
     * \brief Calculate parameters.
     *
     * \param[in] stream_buffer_size Size of buffers of streams.
     * \return Parameters.
     */
    [[nodiscard]] static ClientMemoryParameters calculate_parameters(
        std::size_t stream_buffer_size);

private:
    /*!
     * \brief Get the pointer of a relative address.
     *
     * \param[in] relative_address Relative address.
     * \return Pointer.
     */
    [[nodiscard]] void* at(std::size_t relative_address) const noexcept;

    /*!
     * \brief Get pointers of variables of a stream.
     *
     * \param[in] relative_address Relative address of the stream.
     * \return Arguments of constructors of ShmStreamWriter and ShmStreamReader.
     */
    [[nodiscard]] std::tuple<ShmStreamWriter::AtomicIndex*,
        ShmStreamWriter::AtomicIndex*, messages::BufferView>
    stream_at(std::size_t relative_address) const noexcept;

    //! Starting address of the shared memory.
    void* starting_address_;

    //! Parameters.
    const ClientMemoryParameters* parameters_;
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
