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
 * \brief Definition of ParsedResponse class.
 */
#pragma once

#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"

namespace msgpack_rpc::messages {

/*!
 * \brief Class of parsed responses.
 */
class ParsedResponse {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] id Message ID.
     * \param[in] result Result.
     */
    ParsedResponse(MessageID id, CallResult result)
        : id_(id), result_(std::move(result)) {}

    /*!
     * \brief Get the message ID.
     *
     * \return Message ID.
     */
    [[nodiscard]] MessageID id() const noexcept { return id_; }

    /*!
     * \brief Get the result.
     *
     * \return Result.
     */
    [[nodiscard]] const CallResult& result() const noexcept { return result_; }

private:
    //! Message ID.
    MessageID id_;

    //! Result.
    CallResult result_;
};

}  // namespace msgpack_rpc::messages
