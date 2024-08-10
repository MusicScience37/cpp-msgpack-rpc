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
 * \brief Definition of SharableBinaryHeader struct.
 */
#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <new>

#include "msgpack_rpc/messages/impl/sharable_binary_header_fwd.h"  // IWYU pragma: keep

namespace msgpack_rpc::messages::impl {

/*!
 * \brief Struct of headers of sharable binary data.
 */
struct SharableBinaryHeader {
public:
    //! Total size of the memory buffer allocated for the binary data.
    std::size_t total_memory_size;

    //! Size of the binary data.
    std::size_t binary_size;

    //! Buffer of the object of reference count.
    alignas(alignof(std::atomic<int>))  // NOLINTNEXTLINE
        char reference_count_buffer[sizeof(std::atomic<int>)];

    //! Whether reference count is enabled.
    bool is_reference_count_enabled;
};

//! Alignment of sharable binary data.
constexpr std::size_t SHARABLE_BINARY_ALIGNMENT = alignof(SharableBinaryHeader);

// Assumption to use std::malloc, std::realloc, std::free.
static_assert(SHARABLE_BINARY_ALIGNMENT <= alignof(std::max_align_t));

/*!
 * \brief Allocate a buffer of sharable binary data.
 *
 * \param[in] binary_size Size of the binary data.
 * \return Buffer of sharable binary data.
 */
[[nodiscard]] inline SharableBinaryHeader* allocate_sharable_binary(
    std::size_t binary_size) {
    constexpr std::size_t header_size = sizeof(SharableBinaryHeader);
    const std::size_t total_memory_size = header_size + binary_size;

    void* raw_ptr = std::malloc(total_memory_size);  // NOLINT(*-no-malloc)
    if (raw_ptr == nullptr) {
        // Since memory allocation has failed, the class of exceptions with
        // memory allocation like MsgpackRPCException cannot be used.
        throw std::bad_alloc();
    }

    auto* ptr = static_cast<SharableBinaryHeader*>(raw_ptr);
    ptr->total_memory_size = total_memory_size;
    ptr->binary_size = binary_size;
    ptr->is_reference_count_enabled = false;

    return ptr;
}

/*!
 * \brief Access the reference count of a buffer of sharable binary data.
 *
 * \param[in] buffer Buffer.
 * \return Reference count.
 */
[[nodiscard]] inline std::atomic<int>& reference_count_of(
    SharableBinaryHeader* buffer) noexcept {
    assert(buffer->is_reference_count_enabled);
    // NOLINTNEXTLINE(*-reinterpret-cast)
    return *reinterpret_cast<std::atomic<int>*>(buffer->reference_count_buffer);
}

/*!
 * \brief Deallocate a buffer of sharable binary data.
 *
 * \param[in,out] buffer Buffer.
 */
inline void deallocate_sharable_binary(SharableBinaryHeader* buffer) noexcept {
    if (buffer == nullptr) {
        return;
    }
    if (buffer->is_reference_count_enabled) {
        using AtomicType = std::atomic<int>;
        reference_count_of(buffer).~AtomicType();
    }
    std::free(buffer);  // NOLINT(*-no-malloc)
}

/*!
 * \brief Enable reference count of a buffer of sharable binary data.
 *
 * \param[in,out] buffer Buffer.
 */
inline void enable_reference_count_of_sharable_binary(
    SharableBinaryHeader* buffer) noexcept {
    assert(!buffer->is_reference_count_enabled);
    new (static_cast<void*>(buffer->reference_count_buffer))
        std::atomic<int>(1);
    buffer->is_reference_count_enabled = true;
}

/*!
 * \brief Add a reference count of a buffer of sharable binary data.
 *
 * \param[in,out] buffer Buffer.
 */
inline void add_reference_count_of_sharable_binary(
    SharableBinaryHeader* buffer) noexcept {
    reference_count_of(buffer).fetch_add(1, std::memory_order_relaxed);
}

/*!
 * \brief Decrease a reference count of a buffer of sharable binary data, and
 * deallocate the buffer if possible.
 *
 * \param[in,out] buffer Buffer.
 */
inline void decrease_reference_count_of_sharable_binary(
    SharableBinaryHeader* buffer) noexcept {
    const int reference_count_before =
        reference_count_of(buffer).fetch_sub(1, std::memory_order_acq_rel);
    if (reference_count_before == 1) {
        deallocate_sharable_binary(buffer);
    }
}

/*!
 * \brief Get buffer of binary data from a buffer of sharable binary data.
 *
 * \param[in] buffer Buffer.
 * \return Buffer of binary data.
 */
inline char* binary_buffer_of(SharableBinaryHeader* buffer) noexcept {
    // NOLINTNEXTLINE
    return reinterpret_cast<char*>(buffer) + sizeof(SharableBinaryHeader);
}

}  // namespace msgpack_rpc::messages::impl
