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
 * \brief Definition of IExecutor class.
 */
#pragma once

#include <exception>

#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/operation_type.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Interface of executors to process asynchronous tasks.
 */
class IExecutor {
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
     * \brief Run internal event loops to process asynchronous tasks.
     *
     * \note This function stops
     * - when a task throws an exception, which will be thrown to caller of this
     * function,
     * - when stop function is called.
     */
    virtual void run() = 0;

    /*!
     * \brief Run internal event loops to process asynchronous tasks.
     *
     * \note This function stops
     * - when SIGTERM or SIGINT is received,
     * - when a task throws an exception, which will be thrown to caller of this
     * function,
     * - when stop function is called.
     */
    virtual void run_until_interruption() = 0;

    /*!
     * \brief Stops operation.
     *
     * \note This function returns without waiting stop of operations. So this
     * function can be called from callback functions called in this executor.
     */
    virtual void interrupt() = 0;

    /*!
     * \brief Get the context in asio library.
     *
     * \param[in] type Operation type.
     * \return Context.
     *
     * \note Implementation can return different objects for multiple function
     * calls and different operation types.
     */
    virtual AsioContextType& context(OperationType type) noexcept = 0;

    /*!
     * \brief Get the last exception thrown in asynchronous tasks.
     *
     * \return Pointer of the exception.
     */
    [[nodiscard]] virtual std::exception_ptr last_exception() = 0;

    IExecutor(const IExecutor&) = delete;
    IExecutor(IExecutor&&) = delete;
    IExecutor& operator=(const IExecutor&) = delete;
    IExecutor& operator=(IExecutor&&) = delete;

    //! Destructor.
    virtual ~IExecutor() noexcept = default;

protected:
    //! Constructor.
    IExecutor() noexcept = default;
};

}  // namespace msgpack_rpc::executors
