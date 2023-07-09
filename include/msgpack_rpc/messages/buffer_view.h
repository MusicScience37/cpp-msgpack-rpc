/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of BufferView class.
 */
#pragma once

#include <cstddef>

namespace msgpack_rpc::messages {

/*!
 * \brief Class of buffers without memory management.
 */
class BufferView {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] data Buffer.
     * \param[in] size Buffer size.
     *
     * \note This class saves the pointer to the buffer without memory
     * management.
     */
    BufferView(char* data, std::size_t size) : data_(data), size_(size) {}

    /*!
     * \brief Get the buffer.
     *
     * \return Buffer.
     */
    [[nodiscard]] char* data() const noexcept { return data_; }

    /*!
     * \brief Get the buffer size.
     *
     * \return Buffer size.
     */
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

private:
    //! Buffer.
    char* data_;

    //! Buffer size.
    std::size_t size_;
};

}  // namespace msgpack_rpc::messages
