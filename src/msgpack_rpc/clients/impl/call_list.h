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
 * \brief Definition of CallList class.
 */
#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

#include "msgpack_rpc/clients/impl/call.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_response.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of lists of RPCs.
 */
class CallList {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     */
    explicit CallList(std::shared_ptr<logging::Logger> logger)
        : logger_(std::move(logger)) {}

    /*!
     * \brief Register an RPC.
     *
     * \param[in] method_name Method name.
     * \param[in] parameters Parameters.
     * \return Registered RPC.
     */
    [[nodiscard]] Call& create(messages::MethodNameView method_name,
        const IParametersSerializer& parameters) {
        std::unique_lock<std::mutex> lock(mutex_);
        const messages::MessageID request_id = next_request_id_++;
        const auto [iter, is_success] =
            list_.try_emplace(request_id, request_id,
                parameters.create_serialized_request(method_name, request_id));
        if (!is_success) {
            // This won't occur in the ordinary condition.
            throw MsgpackRPCException(
                StatusCode::UNEXPECTED_ERROR, "Duplicate request ID.");
        }
        return iter->second;
    }

    /*!
     * \brief Handle a response.
     *
     * \param[in] response Response.
     */
    void handle(const messages::ParsedResponse& response) {
        std::unique_lock<std::mutex> lock(mutex_);
        const auto iter = list_.find(response.id());
        if (iter == list_.end()) {
            MSGPACK_RPC_TRACE(logger_,
                "Ignored a response with a non-existing request ID {}.",
                response.id());
            return;
        }
        iter->second.handle(response.result());
    }

private:
    //! List.
    std::unordered_map<messages::MessageID, Call> list_{};

    //! Next request ID.
    messages::MessageID next_request_id_{0};

    //! Mutex of data.
    std::mutex mutex_{};

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::clients::impl
