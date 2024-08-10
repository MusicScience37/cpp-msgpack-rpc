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

#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/messages/impl/sharable_binary_header_fwd.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class of serialized message data.
 *
 * \note Objects of this class shares internal buffers for the same data.
 */
class MSGPACK_RPC_EXPORT SerializedMessage {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] data Pointer to the data.
     * \param[in] size Size of the data.
     */
    SerializedMessage(const char* data, std::size_t size);

    /*!
     * \brief Constructor.
     *
     * \param[in] buffer Buffer.
     *
     *\warning Users must not use this constructor.
     */
    explicit SerializedMessage(impl::SharableBinaryHeader* buffer);

    /*!
     * \brief Copy constructor.
     *
     * \param[in] other Object to copy from.
     */
    SerializedMessage(const SerializedMessage& other) noexcept;

    /*!
     * \brief Move constructor.
     *
     * \param[in,out] other Object to move from.
     */
    SerializedMessage(SerializedMessage&& other) noexcept;

    /*!
     * \brief Copy assignment operator.
     *
     * \param[in] other Object to copy from.
     * \return This object after copy.
     */
    SerializedMessage& operator=(const SerializedMessage& other) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \param[in,out] other Object to move from.
     * \return This object after move.
     */
    SerializedMessage& operator=(SerializedMessage&& other) noexcept;

    /*!
     * \brief Destructor.
     */
    ~SerializedMessage() noexcept;

    /*!
     * \brief Get the pointer to the data.
     *
     * \return Pointer to the data.
     */
    [[nodiscard]] const char* data() const noexcept;

    /*!
     * \brief Get the size of the data.
     *
     * \return Size of the data.
     */
    [[nodiscard]] std::size_t size() const noexcept;

private:
    //! Buffer.
    impl::SharableBinaryHeader* buffer_;
};

}  // namespace msgpack_rpc::messages
