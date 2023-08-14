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

#include <chrono>
#include <exception>
#include <future>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/messages/call_result.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of internal implementation of future objects to get results
 * of RPCs asynchronously.
 */
class CallFutureImpl final : public ICallFutureImpl {
public:
    //! Constructor.
    CallFutureImpl() : future_(promise_.get_future()) {}

    /*!
     * \brief Set a result.
     *
     * \param[in] result Result.
     */
    void set(messages::CallResult result) {
        promise_.set_value(std::move(result));
    }

    /*!
     * \brief Set an error.
     *
     * \param[in] error Error.
     */
    void set(const Status& error) {
        promise_.set_exception(
            std::make_exception_ptr(MsgpackRPCException(error)));
    }

    //! \copydoc msgpack_rpc::clients::impl::ICallFutureImpl::get_result
    [[nodiscard]] messages::CallResult get_result() override {
        return future_.get();
    }

    //! \copydoc msgpack_rpc::clients::impl::ICallFutureImpl::get_result_within
    [[nodiscard]] messages::CallResult get_result_within(
        std::chrono::nanoseconds timeout) override {
        if (future_.wait_for(timeout) != std::future_status::ready) {
            throw MsgpackRPCException(StatusCode::TIMEOUT,
                "Result of an RPC couldn't be received within a timeout.");
        }
        return future_.get();
    }

private:
    //! Promise.
    std::promise<messages::CallResult> promise_;

    //! Future.
    std::future<messages::CallResult> future_;
};

}  // namespace msgpack_rpc::clients::impl
