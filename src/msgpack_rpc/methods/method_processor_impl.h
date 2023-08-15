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
 * \brief Definition of MethodProcessor class.
 */
#pragma once

#include <exception>
#include <memory>
#include <utility>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/parsed_notification.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/methods/i_method.h"
#include "msgpack_rpc/methods/i_method_processor.h"
#include "msgpack_rpc/methods/method_dict.h"

namespace msgpack_rpc::methods {

/*!
 * \brief Class of processor of method calls.
 */
class MethodProcessor final : public IMethodProcessor {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     */
    explicit MethodProcessor(std::shared_ptr<logging::Logger> logger)
        : logger_(std::move(logger)) {}

    //! \copydoc msgpack_rpc::methods::IMethodProcessor::append
    void append(std::unique_ptr<IMethod> method) override {
        methods_.append(std::move(method));
    }

    //! \copydoc msgpack_rpc::methods::IMethodProcessor::call
    [[nodiscard]] messages::SerializedMessage call(
        const messages::ParsedRequest& request) override {
        try {
            return methods_.get(request.method_name())->call(request);
        } catch (const std::exception& e) {
            MSGPACK_RPC_DEBUG(logger_, "Error when calling a method {}: {}",
                request.method_name(), e.what());
            return messages::MessageSerializer::serialize_error_response(
                request.id(), e.what());
        }
    }

    //! \copydoc msgpack_rpc::methods::IMethodProcessor::notify
    void notify(const messages::ParsedNotification& notification) override {
        try {
            methods_.get(notification.method_name())->notify(notification);
        } catch (const std::exception& e) {
            MSGPACK_RPC_DEBUG(logger_,
                "Error when notifying to a method {}: {}",
                notification.method_name(), e.what());
        }
    }

private:
    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Dictionary of methods.
    MethodDict methods_{};
};

}  // namespace msgpack_rpc::methods
