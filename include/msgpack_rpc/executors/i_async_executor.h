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
 * \brief Definition of IAsyncExecutor class.
 */
#pragma once

#include "msgpack_rpc/executors/i_executor.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Interface of executors to process asynchronous tasks and the
 * processing can be started asynchronously.
 */
class IAsyncExecutor : public IExecutor {
public:
    /*!
     * \brief Start internal event loops to process asynchronous tasks.
     *
     * \note Internal processing stops
     * - when a task throws an exception, which can be retrieved from
     * last_exception function,
     * - when stop function is called.
     */
    virtual void start() = 0;

    /*!
     * \brief Stops operation.
     *
     * \warning This function stops internal threads. Do not call this function
     * from callbacks called in this executor.
     */
    virtual void stop() = 0;

    /*!
     * \brief Get the last exception thrown in asynchronous tasks.
     *
     * \return Pointer of the exception.
     */
    [[nodiscard]] virtual std::exception_ptr last_exception() = 0;

    IAsyncExecutor(const IAsyncExecutor&) = delete;
    IAsyncExecutor(IAsyncExecutor&&) = delete;
    IAsyncExecutor& operator=(const IAsyncExecutor&) = delete;
    IAsyncExecutor& operator=(IAsyncExecutor&&) = delete;

    //! Destructor.
    ~IAsyncExecutor() noexcept override = default;

protected:
    //! Constructor.
    IAsyncExecutor() noexcept = default;
};

}  // namespace msgpack_rpc::executors
