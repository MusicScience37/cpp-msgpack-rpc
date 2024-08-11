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
#include <utility>

#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/messages/call_result.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class to set results of RPCs.
 */
class CallPromise {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] deadline Deadline of the result of the RPC.
     */
    explicit CallPromise(std::chrono::steady_clock::time_point deadline)
        : future_(std::make_shared<CallFutureImpl>(deadline)) {}

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
};

}  // namespace msgpack_rpc::clients::impl
