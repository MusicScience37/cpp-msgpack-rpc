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
 * \brief Definition of Call class.
 */
#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <utility>

#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/clients/impl/call_promise.h.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of data of RPC.
 */
class Call {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] id Message ID of the request.
     * \param[in] serialized_request Serialized request data.
     * \param[in] executor Executor.
     * \param[in] deadline Deadline of the result of the RPC.
     * \param[in] on_timeout Callback function called when timeout occurs.
     */
    Call(messages::MessageID id,
        std::shared_ptr<messages::SerializedMessage> serialized_request,
        const std::shared_ptr<executors::IExecutor>& executor,
        std::chrono::steady_clock::time_point deadline,
        std::function<void(messages::MessageID)> on_timeout)
        : id_(id),
          serialized_request_(std::move(serialized_request)),
          promise_(std::make_shared<CallPromise>(
              id, executor, deadline, std::move(on_timeout))) {}

    /*!
     * \brief Start processing.
     */
    void start() { promise_->start(); }

    /*!
     * \brief Get the Message ID of the request.
     *
     * \return Message ID of the request.
     */
    [[nodiscard]] messages::MessageID id() const noexcept { return id_; }

    /*!
     * \brief Get the serialized request data.
     *
     * \return Serialized request data.
     */
    [[nodiscard]] const std::shared_ptr<messages::SerializedMessage>&
    serialized_request() const noexcept {
        return serialized_request_;
    }

    /*!
     * \brief Get the future object to set and get the result of this RPC.
     *
     * \return Future object to set and get the result of this RPC.
     */
    [[nodiscard]] std::shared_ptr<CallFutureImpl> future() const noexcept {
        return promise_->future();
    }

    /*!
     * \brief Handle the result.
     *
     * \param[in] result Result.
     */
    void handle(const messages::CallResult& result) { promise_->set(result); }

    /*!
     * \brief Handle an error.
     *
     * \param[in] error Error.
     */
    void handle(const Status& error) { promise_->set(error); }

private:
    //! Message ID of the request.
    messages::MessageID id_;

    //! Serialized request data.
    std::shared_ptr<messages::SerializedMessage> serialized_request_;

    //! Object to set the result of this RPC.
    std::shared_ptr<CallPromise> promise_;
};

}  // namespace msgpack_rpc::clients::impl
