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
 * \brief Declaration of functions to create executors.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Create an executor.
 *
 * \param[in] logger Logger.
 * \param[in] config Configuration.
 * \return Executor.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<IAsyncExecutor>
create_executor(std::shared_ptr<logging::Logger> logger,
    const config::ExecutorConfig& config);

/*!
 * \brief Create a wrapper of an existing executor.
 *
 * \param[in] executor An existing executor.
 * \return Wrapper of the given executor.
 *
 * \note Resulting wrapper won't call start, stop, run functions of the given
 * executor.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<IAsyncExecutor> wrap_executor(
    std::shared_ptr<IExecutor> executor);

/*!
 * \brief Create an executor runs in a single thread.
 *
 * \param[in] logger Logger.
 * \return Executor.
 *
 * \warning This function is meant to be used in tests. So some limitation
 * exists.
 * \note This executor doesn't support start, stop, last_exception functions.
 * \note This executor exits when no task exists.
 */
[[nodiscard]] MSGPACK_RPC_EXPORT std::shared_ptr<ISingleThreadExecutor>
create_single_thread_executor(std::shared_ptr<logging::Logger> logger);

}  // namespace msgpack_rpc::executors
