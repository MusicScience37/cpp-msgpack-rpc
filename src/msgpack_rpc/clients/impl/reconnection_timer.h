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
 * \brief Class of ReconnectionTimer class.
 */
#pragma once

#include <memory>

#include "msgpack_rpc/config/reconnection_config.h"
#include "msgpack_rpc/executors/timer.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc::clients::impl {

/*!
 * \brief Class of timer to sleep before reconnecting.
 */
class ReconnectionTimer {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] logger Logger.
     * \param[in] config Configuration.
     */
    explicit ReconnectionTimer(
        const std::shared_ptr<executors::IExecutor>& executor,
        std::shared_ptr<logging::Logger> logger,
        const config::ReconnectionConfig& config)
        : timer_(executor, executors::OperationType::TRANSPORT),
          logger_(std::move(logger)),
          config_(config) {}

    /*!
     * \brief Asynchronously wait until the next reconnection.
     *
     * \tparam Function Type of the function called when the time elapsed.
     * \param[in] function Function called when the time elapsed.
     */
    template <typename Function>
    void async_wait(Function&& function) {
        const auto wait_time = config_.wait_time();

        MSGPACK_RPC_WARN(logger_,
            "Failed to connect to all URIs, so retry after {} seconds.",
            std::chrono::duration_cast<std::chrono::duration<double>>(wait_time)
                .count());

        timer_.async_sleep_for(wait_time, std::forward<Function>(function));
    }

    /*!
     * \brief Cancel this timer.
     */
    void cancel() { timer_.cancel(); }

private:
    //! Timer.
    executors::Timer timer_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Configuration.
    config::ReconnectionConfig config_;
};

}  // namespace msgpack_rpc::clients::impl
