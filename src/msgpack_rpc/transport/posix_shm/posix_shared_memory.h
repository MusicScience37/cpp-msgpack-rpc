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
 * \brief Definition of PosixSharedMemory class.
 */
#pragma once

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cstddef>
#include <string_view>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Class of shared memory in POSIX.
 */
class MSGPACK_RPC_EXPORT PosixSharedMemory {
public:
    //! Tag to specify to open an existing shared memory.
    struct OpenExistingTag {};

    //! Tag to specify to open an existing shared memory.
    static inline const OpenExistingTag OPEN_EXISTING{};

    //! Tag to specify to open and initialize a shared memory creating it if not found.
    struct InitializeTag {};

    //! Tag to specify to open and initialize a shared memory creating it if not found.
    static inline const InitializeTag INITIALIZE{};

    /*!
     * \brief Constructor. (Open an existing shared memory.)
     *
     * \param[in] file_name File name.
     */
    PosixSharedMemory(std::string_view file_name, OpenExistingTag /*tag*/);

    /*!
     * \brief Constructor. (Open and initialize a shared memory creating it if
     * not found.)
     *
     * \param[in] file_name File name.
     * \param[in] size Size of the shared memory.
     */
    PosixSharedMemory(
        std::string_view file_name, std::size_t size, InitializeTag /*tag*/);

    PosixSharedMemory(const PosixSharedMemory&) = delete;
    PosixSharedMemory& operator=(const PosixSharedMemory&) = delete;

    /*!
     * \brief Move constructor.
     *
     * \param[in] other Object to move from.
     */
    PosixSharedMemory(PosixSharedMemory&& other) noexcept;

    PosixSharedMemory& operator=(PosixSharedMemory&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~PosixSharedMemory() noexcept;

    /*!
     * \brief Get the pointer to the shared memory.
     *
     * \return Pointer to the shared memory.
     */
    [[nodiscard]] void* get() const noexcept;

    /*!
     * \brief Get the size of the shared memory.
     *
     * \return Size of the shared memory.
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /*!
     * \brief Remove a shared memory.
     *
     * \param[in] file_name File name of the shared memory.
     *
     * \retval true Succeeded.
     * \retval false Failed.
     */
    static bool remove(std::string_view file_name);

private:
    void close() noexcept;

    //! File descriptor. (Negative number when no file is opened by this object.)
    int file_descriptor_{-1};

    //! Pointer to the shared memory.
    void* ptr_{nullptr};

    //! Size of the shared memory.
    std::size_t size_{0};
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
