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

#include <cassert>
#include <chrono>
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "msgpack_rpc/clients/impl/call.h"
#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/clients/impl/request_id_generator.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_response.h"
#include "msgpack_rpc/messages/serialized_message.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of lists of RPCs.
 */
class CallList : public std::enable_shared_from_this<CallList> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] timeout Timeout of RPCs.
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     */
    explicit CallList(std::chrono::nanoseconds timeout,
        std::weak_ptr<executors::IExecutor> executor,
        std::shared_ptr<logging::Logger> logger)
        : timeout_(timeout),
          executor_(std::move(executor)),
          logger_(std::move(logger)) {}

    /*!
     * \brief Register an RPC.
     *
     * \param[in] method_name Method name.
     * \param[in] parameters Parameters.
     * \return Request ID, serialized request, and future object.
     */
    [[nodiscard]] std::tuple<messages::MessageID, messages::SerializedMessage,
        std::shared_ptr<CallFutureImpl>>
    create(messages::MethodNameView method_name,
        const IParametersSerializer& parameters) {
        const auto deadline = std::chrono::steady_clock::now() + timeout_;

        const messages::MessageID request_id = request_id_generator_.generate();
        const auto serialized_request =
            parameters.create_serialized_request(method_name, request_id);

        std::unique_lock<std::mutex> lock(mutex_);
        const auto [iter, is_success] =
            list_.try_emplace(request_id, executor(), deadline);
        if (!is_success) {
            // This won't occur in the ordinary condition.
            throw MsgpackRPCException(
                StatusCode::UNEXPECTED_ERROR, "Duplicate request ID.");
        }
        // Iterator can be invalidated, but reference won't be invalidated in
        // std::unordered_map unless the referenced object is destroyed.
        auto& call = iter->second;
        lock.unlock();

        call.set_timeout(
            deadline, [weak_self = this->weak_from_this(), request_id] {
                const auto self = weak_self.lock();
                if (self) {
                    self->on_timeout(request_id);
                }
            });

        return {request_id, serialized_request, call.future()};
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
        iter->second.set(response.result());
        list_.erase(iter);
    }

private:
    /*!
     * \brief Get the executor.
     *
     * \return Executor.
     */
    [[nodiscard]] std::shared_ptr<executors::IExecutor> executor() {
        auto res = executor_.lock();
        // This won't occur without a bug.
        assert(res);
        return res;
    }

    /*!
     * \brief Handle timeout of a RPC.
     *
     * \param[in] request_id Message ID of the request of the RPC.
     */
    void on_timeout(messages::MessageID request_id) {
        MSGPACK_RPC_WARN(
            logger_, "Timeout of an RPC (request ID: {}).", request_id);
        std::unique_lock<std::mutex> lock(mutex_);
        const auto iter = list_.find(request_id);
        if (iter == list_.end()) {
            return;
        }
        iter->second.set(Status(StatusCode::TIMEOUT,
            "Result of an RPC couldn't be received within a timeout."));
        list_.erase(iter);
    }

    //! List.
    std::unordered_map<messages::MessageID, Call> list_{};

    //! Generator of message IDs of requests.
    RequestIDGenerator request_id_generator_{};

    //! Mutex of data.
    std::mutex mutex_{};

    //! Timeout of RPCs.
    std::chrono::nanoseconds timeout_;

    //! Executor.
    std::weak_ptr<executors::IExecutor> executor_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::clients::impl
