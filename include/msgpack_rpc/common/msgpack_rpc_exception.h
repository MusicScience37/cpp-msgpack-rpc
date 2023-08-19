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
 * \brief Definition of MsgpackRPCException class.
 */
#pragma once

#include <stdexcept>
#include <string_view>

#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"

namespace msgpack_rpc {
inline namespace common {

/*!
 * \brief Class of exceptions in cpp-msgpack-rpc library.
 */
class MSGPACK_RPC_EXPORT MsgpackRPCException : public std::runtime_error {
public:
    // Exceptions must be exported in libraries. If not exported, exceptions
    // cannot be caught correctly.

    /*!
     * \brief Constructor.
     *
     * \param[in] status Status.
     */
    explicit MsgpackRPCException(Status status);

    /*!
     * \brief Constructor.
     *
     * \param[in] code Status code.
     * \param[in] message Error message.
     */
    MsgpackRPCException(StatusCode code, std::string_view message);

    /*!
     * \brief Get the status.
     *
     * \return Status.
     */
    [[nodiscard]] const Status& status() const noexcept;

    /*!
     * \brief Copy constructor.
     */
    MsgpackRPCException(const MsgpackRPCException&) noexcept;

    /*!
     * \brief Move constructor.
     */
    MsgpackRPCException(MsgpackRPCException&&) noexcept;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    MsgpackRPCException& operator=(const MsgpackRPCException&) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    MsgpackRPCException& operator=(MsgpackRPCException&&) noexcept;

    /*!
     * \brief Destructor.
     */
    ~MsgpackRPCException() override;

private:
    //! Error.
    Status status_;
};

}  // namespace common
}  // namespace msgpack_rpc
