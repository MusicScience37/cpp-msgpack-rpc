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

namespace msgpack_rpc {
inline namespace common {

/*!
 * \brief Class of exceptions in cpp-msgpack-rpc library.
 */
class MsgpackRPCException : public std::runtime_error {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] status Status.
     */
    explicit MsgpackRPCException(Status status)
        : std::runtime_error(status.message().data()),
          status_(std::move(status)) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] code Status code.
     * \param[in] message Error message.
     */
    MsgpackRPCException(StatusCode code, std::string_view message)
        : MsgpackRPCException(Status(code, message)) {}

    /*!
     * \brief Get the status.
     *
     * \return Status.
     */
    [[nodiscard]] const Status& status() const noexcept { return status_; }

private:
    //! Error.
    Status status_;
};

}  // namespace common
}  // namespace msgpack_rpc
