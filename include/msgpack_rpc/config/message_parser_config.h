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
 * \brief Definition of MessageParserConfig class.
 */
#pragma once

#include <cstddef>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::config {

/*!
 * \brief Class of configuration of parsers of messages.
 */
class MSGPACK_RPC_EXPORT MessageParserConfig {
public:
    /*!
     * \brief Constructor.
     */
    MessageParserConfig();

    /*!
     * \brief Set the buffer size to read at once.
     *
     * \param[in] value Buffer size to read at once.
     * \return This.
     */
    MessageParserConfig& read_buffer_size(std::size_t value);

    /*!
     * \brief Get the buffer size to read at once.
     *
     * \return Buffer size to read at once.
     */
    [[nodiscard]] std::size_t read_buffer_size() const noexcept;

private:
    //! Buffer size to read at once.
    std::size_t read_buffer_size_;
};

}  // namespace msgpack_rpc::config
