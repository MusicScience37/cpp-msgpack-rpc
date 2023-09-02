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
 * \brief Definition of SerializedMessage class.
 */
#pragma once

#include <cstddef>
#include <string>

namespace msgpack_rpc::messages {

/*!
 * \brief Class of serialized message data.
 */
class SerializedMessage {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] data Pointer to the data.
     * \param[in] size Size of the data.
     */
    SerializedMessage(const char* data, std::size_t size) : data_(data, size) {}

    /*!
     * \brief Get the pointer to the data.
     *
     * \return Pointer to the data.
     */
    [[nodiscard]] const char* data() const noexcept { return data_.data(); }

    /*!
     * \brief Get the size of the data.
     *
     * \return Size of the data.
     */
    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }

private:
    //! Data.
    std::string data_;
};

}  // namespace msgpack_rpc::messages
