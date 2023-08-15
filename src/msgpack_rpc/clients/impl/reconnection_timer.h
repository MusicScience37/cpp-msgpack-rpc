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

#include <chrono>
#include <memory>
#include <random>
#include <utility>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/reconnection_config.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/executors/timer.h"
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
    ReconnectionTimer(const std::shared_ptr<executors::IExecutor>& executor,
        std::shared_ptr<logging::Logger> logger,
        const config::ReconnectionConfig& config)
        : timer_(executor, executors::OperationType::TRANSPORT),
          logger_(std::move(logger)),
          initial_waiting_time_(config.initial_waiting_time()),
          max_waiting_time_(config.max_waiting_time()),
          random_(std::random_device()()),
          jitter_time_dist_(0, config.max_jitter_waiting_time().count()),
          next_wait_time_without_jitter_(config.initial_waiting_time()) {
        // This cannot be checked in ReconnectionConfig.
        if (max_waiting_time_ < initial_waiting_time_) {
            throw MsgpackRPCException(StatusCode::INVALID_ARGUMENT,
                "The maximum waiting time must be longer than of equal to the "
                "initial waiting time.");
        }
    }

    /*!
     * \brief Asynchronously wait until the next reconnection.
     *
     * \tparam Function Type of the function called when the time elapsed.
     * \param[in] function Function called when the time elapsed.
     */
    template <typename Function>
    void async_wait(Function&& function) {
        const auto wait_time = compute_waiting_time();

        MSGPACK_RPC_WARN(logger_,
            "Failed to connect to all URIs, so retry after {:.3f} seconds.",
            std::chrono::duration_cast<std::chrono::duration<double>>(wait_time)
                .count());

        timer_.async_sleep_for(wait_time, std::forward<Function>(function));
    }

    /*!
     * \brief Reset the waiting time.
     *
     * \note Call this function if a connection is established successfully
     * once.
     */
    void reset() { next_wait_time_without_jitter_ = initial_waiting_time_; }

    /*!
     * \brief Cancel this timer.
     */
    void cancel() { timer_.cancel(); }

private:
    /*!
     * \brief Compute the waiting time.
     *
     * \return Waiting time.
     */
    [[nodiscard]] std::chrono::nanoseconds compute_waiting_time() {
        std::chrono::nanoseconds wait_time = next_wait_time_without_jitter_;
        wait_time += std::chrono::nanoseconds(jitter_time_dist_(random_));

        next_wait_time_without_jitter_ *= 2;
        if (next_wait_time_without_jitter_ > max_waiting_time_) {
            next_wait_time_without_jitter_ = max_waiting_time_;
        }

        return wait_time;
    }

    //! Timer.
    executors::Timer timer_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Initial waiting time.
    std::chrono::nanoseconds initial_waiting_time_;

    //! Maximum waiting time.
    std::chrono::nanoseconds max_waiting_time_;

    //! Random number generator.
    std::mt19937 random_;

    //! Distribution of jitter time.
    std::uniform_int_distribution<std::chrono::nanoseconds::rep>
        jitter_time_dist_;

    //! Next waiting time without jitter.
    std::chrono::nanoseconds next_wait_time_without_jitter_;
};

}  // namespace msgpack_rpc::clients::impl
