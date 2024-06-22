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
 * \brief Implementation of Status class.
 */
#include "msgpack_rpc/common/status.h"

#include <memory>
#include <string>

#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc {
inline namespace common {

//! Internal data.
struct Status::Data {
    //! Status code.
    StatusCode code;

    //! Error message.
    std::string message;

    /*!
     * \brief Create data.
     *
     * \param[in] code Status code.
     * \param[in] message Error message.
     * \return Data.
     */
    static std::shared_ptr<Data> create(
        StatusCode code, std::string_view message) {
        if (code == StatusCode::SUCCESS) {
            return nullptr;
        }
        return std::make_shared<Data>(Data{code, std::string(message)});
    }
};

Status::Status() noexcept = default;

Status::Status(StatusCode code, std::string_view message)
    : data_(Data::create(code, message)) {}

Status::Status(const Status&) noexcept = default;

Status::Status(Status&&) noexcept = default;

Status& Status::operator=(const Status&) noexcept = default;

Status& Status::operator=(Status&&) noexcept = default;

Status::~Status() noexcept = default;

StatusCode Status::code() const noexcept {
    if (!data_) {
        return StatusCode::SUCCESS;
    }
    return data_->code;
}

std::string_view Status::message() const noexcept {
    if (!data_) {
        static constexpr auto empty = std::string_view("");
        return empty;
    }
    return data_->message;
}

}  // namespace common
}  // namespace msgpack_rpc
