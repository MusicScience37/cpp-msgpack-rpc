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

#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/operation_type.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Interface of executors to process asynchronous tasks.
 */
class IExecutor {
public:
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
