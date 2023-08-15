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

namespace msgpack_rpc::clients {

/*!
 * \brief Class of exceptions specifying errors in servers.
 */
class ServerException : public MsgpackRPCException {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] object Object in msgpack library specifying the error.
     * \param[in] zone Zone in msgpack library.
     */
    ServerException(msgpack::object object, std::shared_ptr<msgpack::zone> zone)
        : MsgpackRPCException(
              StatusCode::SERVER_ERROR, "An error in a server."),
          zone_(std::move(zone)),
          object_(object) {}

    /*!
     * \brief Get the object in msgpack library.
     *
     * \return Object in msgpack library.
     */
    [[nodiscard]] const msgpack::object& object() const noexcept {
        return object_;
    }

    /*!
     * \brief Get the error.
     *
     * \tparam T Type.
     * \return Error.
     */
    template <typename T>
    [[nodiscard]] T error_as() const {
        try {
            return object_.as<T>();
        } catch (const msgpack::type_error&) {
            throw MsgpackRPCException(
                StatusCode::TYPE_ERROR, "Invalid type of the error.");
        }
    }

    /*!
     * \brief Copy constructor.
     */
    ServerException(const ServerException&) noexcept = default;

    /*!
     * \brief Move constructor.
     */
    ServerException(ServerException&&) noexcept = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    ServerException& operator=(const ServerException&) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    ServerException& operator=(ServerException&&) noexcept = default;

    //! Destructor.
    ~ServerException() noexcept override = default;

private:
    //! Zone in msgpack library.
    std::shared_ptr<msgpack::zone> zone_;

    //! Object in msgpack library specifying the error.
    msgpack::object object_;
};

}  // namespace msgpack_rpc::clients
