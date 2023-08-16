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
 * \brief Definition of CallPromise class.
 */
#pragma once

#include <chrono>
#include <memory>

#include "msgpack_rpc/clients/impl/call_future_impl.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class to set results of RPCs.
 */
class CallPromise : public std::enable_shared_from_this<CallPromise> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] request_id Message ID of the request of the RPC.
     * \param[in] executor Executor.
     * \param[in] deadline Deadline of the result of the RPC.
     * \param[in] on_timeout Callback function called when timeout occurs.
     */
    CallPromise(messages::MessageID request_id,
        const std::shared_ptr<executors::IExecutor>& executor,
        std::chrono::steady_clock::time_point deadline,
        std::function<void(messages::MessageID)> on_timeout)
        : future_(std::make_shared<CallFutureImpl>(deadline)),
          request_id_(request_id),
          timer_(executor, executors::OperationType::CALLBACK),
          deadline_(deadline),
          on_timeout_(std::move(on_timeout)) {}

    /*!
     * \brief Start processing.
     */
    void start() {
        timer_.async_sleep_until(
            deadline_, [weak_self = this->weak_from_this()] {
                const auto self = weak_self.lock();
                if (self) {
                    self->set(Status(StatusCode::TIMEOUT,
                        "Result of an RPC couldn't be received within a "
                        "timeout."));
                    self->on_timeout_(self->request_id_);
                }
            });
    }

    /*!
     * \brief Set a result.
     *
     * \param[in] result Result.
     */
    void set(messages::CallResult result) { future_->set(std::move(result)); }

    /*!
     * \brief Set an error.
     *
     * \param[in] error Error.
     */
    void set(const Status& error) { future_->set(error); }

    /*!
     * \brief Get the future.
     *
     * \return Future.
     */
    [[nodiscard]] std::shared_ptr<CallFutureImpl> future() const noexcept {
        return future_;
    }

private:
    //! Future.
    std::shared_ptr<CallFutureImpl> future_;

    //! Message ID of the request of the RPC.
    messages::MessageID request_id_;

    //! Timer to check timeout.
    executors::Timer timer_;

    //! Deadline of the result of the RPC.
    std::chrono::steady_clock::time_point deadline_;

    //! Callback function called when timeout occurs.
    std::function<void(messages::MessageID)> on_timeout_;
};

}  // namespace msgpack_rpc::clients::impl
