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

#include <memory>
#include <unordered_map>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_serializer.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/methods/i_method.h"
#include "msgpack_rpc/methods/i_method_processor.h"

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
        const messages::MethodNameView method_name = method->name();
        const auto result =
            methods_.try_emplace(method_name, std::move(method));
        if (!result.second) {
            throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
                fmt::format("Duplicate method name {}.", method_name));
        }
    }

    //! \copydoc msgpack_rpc::methods::IMethodProcessor::call
    [[nodiscard]] messages::SerializedMessage call(
        const messages::ParsedRequest& request) override {
        const auto iter = methods_.find(request.method_name());
        if (iter == methods_.end()) {
            const auto message =
                fmt::format("Method {} not found.", request.method_name());
            MSGPACK_RPC_DEBUG(logger_, message);
            return messages::MessageSerializer::serialize_error_response(
                request.id(), message);
        }
        return iter->second->call(request);
    }

    //! \copydoc msgpack_rpc::methods::IMethodProcessor::notify
    void notify(const messages::ParsedNotification& notification) override {
        const auto iter = methods_.find(notification.method_name());
        if (iter == methods_.end()) {
            const auto message =
                fmt::format("Method {} not found.", notification.method_name());
            MSGPACK_RPC_DEBUG(logger_, message);
            return;
        }
        iter->second->notify(notification);
    }

private:
    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Dictionary of methods.
    std::unordered_map<messages::MethodNameView, std::unique_ptr<IMethod>>
        methods_{};
};

}  // namespace msgpack_rpc::methods
