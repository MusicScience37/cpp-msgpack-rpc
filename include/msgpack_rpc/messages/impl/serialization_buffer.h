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
 * \brief Definition of SerializationBuffer class.
 */
#pragma once

#include <cstddef>

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/messages/impl/sharable_binary_header_fwd.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::messages::impl {

/*!
 * \brief Class of buffers for serialization.
 *
 * \warning This class is designed only for internal use.
 */
class MSGPACK_RPC_EXPORT SerializationBuffer {
public:
    /*!
     * \brief Constructor.
     */
    SerializationBuffer();

    /*!
     * \brief Destructor.
     */
    ~SerializationBuffer() noexcept;

    SerializationBuffer(const SerializationBuffer&) = delete;
    SerializationBuffer(SerializationBuffer&&) = delete;
    SerializationBuffer& operator=(const SerializationBuffer&) = delete;
    SerializationBuffer& operator=(SerializationBuffer&&) = delete;

    /*!
     * \brief Write data to the buffer.
     *
     * \param[in] data
     * \param[in] size
     */
    void write(const char* data, std::size_t size);

    /*!
     * \brief Release the buffer as msgpack_rpc::messages::SerializedMessage
     * object.
     *
     * \return Released buffer.
     *
     * \warning After call of this function, this object cannot be used any
     * more.
     */
    [[nodiscard]] SerializedMessage release() noexcept;

private:
    //! Buffer.
    impl::SharableBinaryHeader* buffer_;
};

}  // namespace msgpack_rpc::messages::impl
