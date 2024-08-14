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
 * \brief Implementation of PosixSharedMemory class.
 */
#include "msgpack_rpc/transport/posix_shm/posix_shared_memory.h"

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <cerrno>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

#include <fcntl.h>
#include <fmt/format.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/transport/posix_shm/get_errno_message.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Get the size of a shared memory.
 *
 * \param[in] file_descriptor File descriptor.
 * \param[in] file_name File name of the shared memory. (For error message.)
 * \return Size.
 */
[[nodiscard]] std::size_t get_shared_memory_size(
    int file_descriptor, std::string_view file_name) {
    struct stat data {};
    if (fstat(file_descriptor, &data) != 0) {
        const int errno_val = errno;
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to get the size of shared memory {}: {}",
                file_name, get_errno_message(errno_val)));
    }
    return static_cast<std::size_t>(data.st_size);
}

[[nodiscard]] void* map_shared_memory(
    int file_descriptor, std::size_t size, std::string_view file_name) {
    void* address = nullptr;
    constexpr int protection = PROT_READ | PROT_WRITE;
    constexpr int flags = MAP_SHARED;
    constexpr off_t offset = 0;
    void* ptr = mmap(address, size, protection, flags, file_descriptor, offset);
    if (ptr == MAP_FAILED) {
        const int errno_val = errno;
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to open shared memory {}: {}", file_name,
                get_errno_message(errno_val)));
    }
    return ptr;
}

PosixSharedMemory::PosixSharedMemory(
    std::string_view file_name, OpenExistingTag /*tag*/) {
    const std::string shm_full_name = fmt::format("/{}", file_name);
    constexpr int flag = O_RDWR;
    // TODO This may need to be configurable.
    constexpr auto mode = static_cast<mode_t>(0666);
    file_descriptor_ = shm_open(shm_full_name.c_str(), flag, mode);
    if (file_descriptor_ < 0) {
        const int errno_val = errno;
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to open shared memory {}: {}", file_name,
                get_errno_message(errno_val)));
    }

    size_ = get_shared_memory_size(file_descriptor_, file_name);

    ptr_ = map_shared_memory(file_descriptor_, size_, file_name);
}

PosixSharedMemory::PosixSharedMemory(
    std::string_view file_name, std::size_t size, InitializeTag /*tag*/) {
    const std::string shm_full_name = fmt::format("/{}", file_name);
    constexpr int flag = O_RDWR | O_CREAT;
    // TODO This may need to be configurable.
    constexpr auto mode = static_cast<mode_t>(0600);
    file_descriptor_ = shm_open(shm_full_name.c_str(), flag, mode);
    if (file_descriptor_ < 0) {
        const int errno_val = errno;
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to open shared memory {}: {}", file_name,
                get_errno_message(errno_val)));
    }

    if (ftruncate(file_descriptor_, static_cast<off_t>(size)) != 0) {
        const int errno_val = errno;
        throw MsgpackRPCException(StatusCode::OPERATION_FAILURE,
            fmt::format("Failed to set the size of a shared memory: {}",
                get_errno_message(errno_val)));
    }
    size_ = size;

    ptr_ = map_shared_memory(file_descriptor_, size_, file_name);
}

PosixSharedMemory::PosixSharedMemory(PosixSharedMemory&& other) noexcept
    : file_descriptor_(std::exchange(other.file_descriptor_, -1)),
      ptr_(std::exchange(other.ptr_, nullptr)),
      size_(std::exchange(other.size_, 0)) {}

PosixSharedMemory::~PosixSharedMemory() noexcept { close(); }

void* PosixSharedMemory::get() const noexcept { return ptr_; }

std::size_t PosixSharedMemory::size() const noexcept { return size_; }

void PosixSharedMemory::close() noexcept {
    if (file_descriptor_ < 0) {
        munmap(ptr_, size_);
        ::close(file_descriptor_);
        file_descriptor_ = -1;
    }
}

bool PosixSharedMemory::remove(std::string_view file_name) {
    const std::string shm_full_name = fmt::format("/{}", file_name);
    return shm_unlink(shm_full_name.c_str()) == 0;
}

}  // namespace msgpack_rpc::transport::posix_shm

#endif
