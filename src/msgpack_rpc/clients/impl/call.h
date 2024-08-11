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
#include <memory>
#include <utility>

#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/clients/impl/call_promise.h.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/executors/timer.h"
#include "msgpack_rpc/messages/call_result.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of data of RPC.
 */
class Call {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] deadline Deadline of the result of the RPC.
     */
    Call(const std::shared_ptr<executors::IExecutor>& executor,
        std::chrono::steady_clock::time_point deadline)
        : promise_(deadline),
          timeout_timer_(executor, executors::OperationType::CALLBACK) {}

    /*!
     * \brief Set timeout.
     *
     * \tparam OnTimeout Type of the function to handle timeout.
     * \param[in] deadline Deadline.
     * \param[in] on_timeout Function to handle timeout.
     */
    template <typename OnTimeout>
    void set_timeout(std::chrono::steady_clock::time_point deadline,
        OnTimeout&& on_timeout) {
        timeout_timer_.async_sleep_until(
            deadline, std::forward<OnTimeout>(on_timeout));
    }

    /*!
     * \brief Get the future object to set and get the result of this RPC.
     *
     * \return Future object to set and get the result of this RPC.
     */
    [[nodiscard]] std::shared_ptr<CallFutureImpl> future() const noexcept {
        return promise_.future();
    }

    /*!
     * \brief Set the result.
     *
     * \param[in] result Result.
     */
    void set(const messages::CallResult& result) { promise_.set(result); }

    /*!
     * \brief Set an error.
     *
     * \param[in] error Error.
     */
    void set(const Status& error) { promise_.set(error); }

private:
    //! Object to set the result of this RPC.
    CallPromise promise_;

    //! Timer of timeout.
    executors::Timer timeout_timer_;
};

}  // namespace msgpack_rpc::clients::impl
