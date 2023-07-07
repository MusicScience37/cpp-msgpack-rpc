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
 * \brief Definition of Status class.
 */
#pragma once

#include <memory>
#include <string_view>

#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc {
inline namespace common {

/*!
 * \brief Class of statuses.
 */
class Status {
public:
    /*!
     * \brief Constructor. (Set to success.)
     */
    Status() noexcept;

    /*!
     * \brief Constructor.
     *
     * \param[in] code Status code.
     * \param[in] message Error message.
     */
    Status(StatusCode code, std::string_view message);

    /*!
     * \brief Copy constructor.
     */
    Status(const Status&) noexcept;

    /*!
     * \brief Move constructor.
     */
    Status(Status&&) noexcept;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    Status& operator=(const Status&) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    Status& operator=(Status&&) noexcept;

    /*!
     * \brief Destructor.
     */
    ~Status() noexcept;

    /*!
     * \brief Get the status code.
     *
     * \return Status code.
     */
    [[nodiscard]] StatusCode code() const noexcept;

    /*!
     * \brief Get the error message.
     *
     * \return Error message.
     *
     * \note This returns empty message when this status is not an error.
     */
    [[nodiscard]] std::string_view message() const noexcept;

private:
    //! Internal data.
    struct Data;

    //! Internal data.
    std::shared_ptr<Data> data_;
};

}  // namespace common
}  // namespace msgpack_rpc
