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
 * \brief Definition of ReceivedMessageProcessor class.
 */
#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

#include "msgpack_rpc/clients/impl/call_list.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc/util/format_msgpack_object.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class to process received messages.
 */
class ReceivedMessageProcessor {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     * \param[in] call_list List of RPCs.
     */
    ReceivedMessageProcessor(std::shared_ptr<logging::Logger> logger,
        std::shared_ptr<CallList> call_list)
        : logger_(std::move(logger)), call_list_(std::move(call_list)) {}

    /*!
     * \brief Process a received message.
     *
     * \param[in] message Message.
     */
    void operator()(const messages::ParsedMessage& message) {
        const auto* response = std::get_if<messages::ParsedResponse>(&message);
        if (response == nullptr) {
            MSGPACK_RPC_WARN(logger_, "Received an invalid message.");
            return;
        }
        if (response->result().is_success()) {
            MSGPACK_RPC_DEBUG(logger_, "Received successful response (id: {})",
                response->id());
        } else {
            MSGPACK_RPC_DEBUG(logger_, "Received error response (id: {}): {}",
                response->id(),
                util::format_msgpack_object(response->result().object()));
        }
        call_list_->handle(*response);
    }

private:
    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! List of RPCs.
    std::shared_ptr<CallList> call_list_;
};

}  // namespace msgpack_rpc::clients::impl
