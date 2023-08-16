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
 * \brief Definition of CallFutureImpl class.
 */
#pragma once

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/executors/timer.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of internal implementation of future objects to get results
 * of RPCs asynchronously.
 */
class CallFutureImpl final
    : public ICallFutureImpl,
      public std::enable_shared_from_this<CallFutureImpl> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] deadline Deadline of the result of the RPC.
     */
    explicit CallFutureImpl(std::chrono::steady_clock::time_point deadline)
        : deadline_(deadline) {}

    /*!
     * \brief Set a result.
     *
     * \param[in] result Result.
     */
    void set(messages::CallResult result) {
        std::unique_lock<std::mutex> lock(is_set_mutex_);
        if (is_set_) {
            return;
        }

        result_.emplace(std::move(result));

        is_set_ = true;
        lock.unlock();
        is_set_cond_var_.notify_all();
    }

    /*!
     * \brief Set an error.
     *
     * \param[in] error Error.
     */
    void set(const Status& error) {
        std::unique_lock<std::mutex> lock(is_set_mutex_);
        if (is_set_) {
            return;
        }

        if (error.code() == StatusCode::SUCCESS) {
            throw MsgpackRPCException(
                StatusCode::INVALID_ARGUMENT, "Invalid error status.");
        }
        status_ = error;

        is_set_ = true;
        lock.unlock();
        is_set_cond_var_.notify_all();
    }

    //! \copydoc msgpack_rpc::clients::impl::ICallFutureImpl::get_result
    [[nodiscard]] messages::CallResult get_result() override {
        wait();
        return get_result_impl();
    }

    //! \copydoc msgpack_rpc::clients::impl::ICallFutureImpl::get_result_within
    [[nodiscard]] messages::CallResult get_result_within(
        std::chrono::nanoseconds timeout) override {
        wait_for(timeout);
        return get_result_impl();
    }

private:
    /*!
     * \brief Wait the result.
     */
    void wait() { wait_until_impl(deadline_); }

    /*!
     * \brief Wait the result for the given time.
     *
     * \param[in] timeout Duration of timeout of waiting.
     */
    void wait_for(std::chrono::nanoseconds timeout) {
        const auto deadline = std::min<std::chrono::steady_clock::time_point>(
            std::chrono::steady_clock::now() + timeout, deadline_);
        wait_until_impl(deadline);
    }

    /*!
     * \brief Wait the result until the given deadline without consideration of
     * the deadline of the RPC.
     *
     * \param[in] deadline Deadline of waiting.
     */
    void wait_until_impl(std::chrono::steady_clock::time_point deadline) {
        std::unique_lock<std::mutex> lock(is_set_mutex_);
        if (!is_set_cond_var_.wait_until(
                lock, deadline, [this] { return is_set_; })) {
            throw MsgpackRPCException(StatusCode::TIMEOUT,
                "Result of an RPC couldn't be received within a timeout.");
        }
    }

    /*!
     * \brief Get the result assuming the result is already set.
     *
     * \return Result.
     */
    [[nodiscard]] messages::CallResult get_result_impl() {
        if (result_) {
            return *result_;
        }
        throw MsgpackRPCException(status_);
    }

    //! Result of the RPC.
    std::optional<messages::CallResult> result_{};

    //! Status.
    Status status_{};

    //! Whether a result or an error is set.
    bool is_set_{false};

    //! Deadline of the result of the RPC.
    std::chrono::steady_clock::time_point deadline_;

    /*!
     * \brief Mutex of is_set_.
     *
     * - This mutex should be locked for use of is_set_.
     * - If is_set_ is set to true, result_ and status_
     *   can be used without locks.
     */
    std::mutex is_set_mutex_{};

    //! Condition variable for notifying change of is_set_.
    std::condition_variable is_set_cond_var_{};
};

}  // namespace msgpack_rpc::clients::impl
