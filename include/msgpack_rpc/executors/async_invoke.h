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
 * \brief Definition of async_invoke function.
 */
#pragma once

#include <memory>
#include <utility>

#include <asio/io_context.hpp>  // IWYU pragma: export
#include <asio/post.hpp>        // IWYU pragma: export

#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Asynchronously invoke a function.
 *
 * \tparam Function Type of the function.
 * \param[in] executor Executor.
 * \param[in] type Type of the operation.
 * \param[in] function Function to invoke.
 */
template <typename Function>
void async_invoke(const std::shared_ptr<IExecutor>& executor,
    OperationType type, Function&& function) {
    asio::post(executor->context(type), std::forward<Function>(function));
}

}  // namespace msgpack_rpc::executors
