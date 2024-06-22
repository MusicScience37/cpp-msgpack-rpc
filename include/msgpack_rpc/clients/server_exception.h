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
 * \brief Definition of ServerException class.
 */
#pragma once

#include <memory>

#include <msgpack.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc::clients {

/*!
 * \brief Class of exceptions specifying errors in servers.
 */
class MSGPACK_RPC_EXPORT ServerException : public MsgpackRPCException {
public:
    // Exceptions must be exported in libraries. If not exported, exceptions
    // cannot be caught correctly.

    /*!
     * \brief Constructor.
     *
     * \param[in] object Object in msgpack library specifying the error.
     * \param[in] zone Zone in msgpack library.
     */
    ServerException(
        msgpack::object object, std::shared_ptr<msgpack::zone> zone);

    /*!
     * \brief Get the object in msgpack library.
     *
     * \return Object in msgpack library.
     */
    [[nodiscard]] const msgpack::object& object() const noexcept;

    /*!
     * \brief Get the error.
     *
     * \tparam T Type.
     * \return Error.
     */
    template <typename T>
    [[nodiscard]] T error_as() const {
        try {
            return object().as<T>();
        } catch (const msgpack::type_error&) {
            throw MsgpackRPCException(
                StatusCode::TYPE_ERROR, "Invalid type of the error.");
        }
    }

    /*!
     * \brief Copy constructor.
     */
    ServerException(const ServerException&) noexcept;

    /*!
     * \brief Move constructor.
     */
    ServerException(ServerException&&) noexcept;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    ServerException& operator=(const ServerException&) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    ServerException& operator=(ServerException&&) noexcept;

    //! Destructor.
    ~ServerException() noexcept override;

private:
    //! Zone in msgpack library.
    std::shared_ptr<msgpack::zone> zone_;

    //! Object in msgpack library specifying the error.
    msgpack::object object_;
};

}  // namespace msgpack_rpc::clients
