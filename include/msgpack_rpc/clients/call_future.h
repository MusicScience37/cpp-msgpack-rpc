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
 * \brief Definition of CallFuture class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"

namespace msgpack_rpc::clients {

/*!
 * \brief Class of future object to wait for asynchronous RPCs.
 *
 * \tparam Result Type of the result.
 */
template <typename Result>
class CallFuture {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] impl Object of the internal implementation.
     */
    explicit CallFuture(std::shared_ptr<impl::ICallFutureImpl> impl)
        : impl_(std::move(impl)) {}

    /*!
     * \brief Get the result of RPC.
     *
     * \return Result.
     *
     * \note This function will wait for the result if not received.
     */
    [[nodiscard]] Result get_result() {
        const auto call_result = impl_->get_result();
        return call_result.result_as<Result>();
    }

    /*!
     * \brief Get the result of RPC within a timeout.
     *
     * \param[in] timeout Timeout.
     * \return Result.
     *
     * \note This function will wait for the result if not received, and throw
     * an exception when no result can be received within the given timeout.
     */
    [[nodiscard]] Result get_result_within(std::chrono::nanoseconds timeout) {
        const auto call_result = impl_->get_result_within(timeout);
        return call_result.result_as<Result>();
    }

private:
    //! Object of the internal implementation.
    std::shared_ptr<impl::ICallFutureImpl> impl_;
};

}  // namespace msgpack_rpc::clients
