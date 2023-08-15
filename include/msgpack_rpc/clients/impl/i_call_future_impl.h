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
 * \brief Definition of ICallFutureImpl class.
 */
#pragma once

#include <chrono>

#include "msgpack_rpc/messages/call_result.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Interface of internal implementation of future objects to get results
 * of RPCs asynchronously.
 */
class ICallFutureImpl {
public:
    /*!
     * \brief Get the result of RPC.
     *
     * \return Result.
     *
     * \note This function will wait for the result if not received.
     */
    [[nodiscard]] virtual messages::CallResult get_result() = 0;

    /*!
     * \brief Get the result of RPC within a timeout.
     *
     * \param[in] timeout Timeout.
     * \return Result.
     *
     * \note This function will wait for the result if not received, and throw
     * an exception when no result can be received within the given timeout.
     */
    [[nodiscard]] virtual messages::CallResult get_result_within(
        std::chrono::nanoseconds timeout) = 0;

    ICallFutureImpl(const ICallFutureImpl&) = delete;
    ICallFutureImpl(ICallFutureImpl&&) = delete;
    ICallFutureImpl& operator=(const ICallFutureImpl&) = delete;
    ICallFutureImpl& operator=(ICallFutureImpl&&) = delete;

    //! Destructor.
    virtual ~ICallFutureImpl() noexcept = default;

protected:
    //! Constructor.
    ICallFutureImpl() noexcept = default;
};

}  // namespace msgpack_rpc::clients::impl
