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
 * \brief Test of SharableBinaryHeader struct.
 */
#include "msgpack_rpc/messages/impl/sharable_binary_header.h"

#include <cstring>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::messages::impl::SharableBinaryHeader") {
    using msgpack_rpc::messages::impl::add_reference_count_of_sharable_binary;
    using msgpack_rpc::messages::impl::allocate_sharable_binary;
    using msgpack_rpc::messages::impl::binary_buffer_of;
    using msgpack_rpc::messages::impl::deallocate_sharable_binary;
    using msgpack_rpc::messages::impl::
        decrease_reference_count_of_sharable_binary;
    using msgpack_rpc::messages::impl::
        enable_reference_count_of_sharable_binary;
    using msgpack_rpc::messages::impl::reference_count_of;
    using msgpack_rpc::messages::impl::SharableBinaryHeader;

    SECTION("allocate a buffer") {
        constexpr std::size_t binary_size = 123;
        SharableBinaryHeader* buffer = allocate_sharable_binary(binary_size);

        REQUIRE(static_cast<void*>(buffer) != nullptr);
        CHECK(buffer->binary_size == binary_size);
        CHECK(buffer->total_memory_size > binary_size);
        CHECK_FALSE(buffer->is_reference_count_enabled);

        deallocate_sharable_binary(buffer);
        // Sanitizer will check whether memory was deallocated.
    }

    SECTION("enable reference count") {
        constexpr std::size_t binary_size = 123;
        SharableBinaryHeader* buffer = allocate_sharable_binary(binary_size);
        REQUIRE(static_cast<void*>(buffer) != nullptr);
        CHECK_FALSE(buffer->is_reference_count_enabled);

        enable_reference_count_of_sharable_binary(buffer);
        CHECK(buffer->is_reference_count_enabled);
        CHECK(reference_count_of(buffer).load() == 1);

        deallocate_sharable_binary(buffer);
        // Sanitizer will check whether memory was deallocated.
    }

    SECTION("use reference count") {
        constexpr std::size_t binary_size = 123;
        SharableBinaryHeader* buffer = allocate_sharable_binary(binary_size);
        REQUIRE(static_cast<void*>(buffer) != nullptr);
        enable_reference_count_of_sharable_binary(buffer);
        CHECK(reference_count_of(buffer).load() == 1);

        add_reference_count_of_sharable_binary(buffer);
        CHECK(reference_count_of(buffer).load() == 2);

        add_reference_count_of_sharable_binary(buffer);
        CHECK(reference_count_of(buffer).load() == 3);

        decrease_reference_count_of_sharable_binary(buffer);
        CHECK(reference_count_of(buffer).load() == 2);

        decrease_reference_count_of_sharable_binary(buffer);
        CHECK(reference_count_of(buffer).load() == 1);

        decrease_reference_count_of_sharable_binary(buffer);
        // Sanitizer will check whether memory was deallocated.
    }

    SECTION("write binary data") {
        constexpr std::size_t binary_size = 128 - sizeof(SharableBinaryHeader);
        SharableBinaryHeader* buffer = allocate_sharable_binary(binary_size);
        REQUIRE(static_cast<void*>(buffer) != nullptr);
        CHECK_FALSE(buffer->is_reference_count_enabled);

        const std::vector<char> data(binary_size, 'a');
        std::memcpy(binary_buffer_of(buffer), data.data(), data.size());
        // Sanitizer will check whether the above operation is valid.

        deallocate_sharable_binary(buffer);
        // Sanitizer will check whether memory was deallocated.
    }
}
