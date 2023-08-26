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

#include <chrono>
#include <memory>
#include <type_traits>
#include <utility>

#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/clients/server_exception.h"
#include "msgpack_rpc/messages/call_result.h"

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
        return get_from_call_result(call_result);
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
        return get_from_call_result(call_result);
    }

private:
    /*!
     * \brief Get the result from CallResult object.
     *
     * \param[in] call_result CallResult object.
     * \return Result.
     */
    [[nodiscard]] static Result get_from_call_result(
        const messages::CallResult& call_result) {
        if (call_result.is_success()) {
            return call_result.result_as<Result>();
        }
        throw ServerException(call_result.object(), call_result.zone());
    }

    //! Object of the internal implementation.
    std::shared_ptr<impl::ICallFutureImpl> impl_;
};

/*!
 * \brief Specialization of msgpack_rpc::clients::CallFuture for void type.
 */
template <>
class CallFuture<void> {
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
     * \note This function will wait for the result if not received.
     */
    void get_result() {
        const auto call_result = impl_->get_result();
        get_from_call_result(call_result);
    }

    /*!
     * \brief Get the result of RPC within a timeout.
     *
     * \param[in] timeout Timeout.
     *
     * \note This function will wait for the result if not received, and throw
     * an exception when no result can be received within the given timeout.
     */
    void get_result_within(std::chrono::nanoseconds timeout) {
        const auto call_result = impl_->get_result_within(timeout);
        get_from_call_result(call_result);
    }

private:
    /*!
     * \brief Get the result from CallResult object.
     *
     * \param[in] call_result CallResult object.
     */
    static void get_from_call_result(const messages::CallResult& call_result) {
        if (call_result.is_success()) {
            return;
        }
        throw ServerException(call_result.object(), call_result.zone());
    }

    //! Object of the internal implementation.
    std::shared_ptr<impl::ICallFutureImpl> impl_;
};

}  // namespace msgpack_rpc::clients
