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
 * \brief Definition of Timer class.
 */
#pragma once

#include <chrono>
#include <memory>

#include <asio/error_code.hpp>
#include <asio/steady_timer.hpp>

#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Class of timers to call functions later.
 */
class Timer {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor Executor.
     * \param[in] type Type of the operation.
     */
    Timer(const std::shared_ptr<IExecutor>& executor, OperationType type)
        : timer_(executor->context(type)) {}

    /*!
     * \brief Asynchronously sleep until a time point.
     *
     * \tparam Function Type of the function.
     * \param[in] time Time point.
     * \param[in] function Function called when the time come.
     */
    template <typename Function>
    void async_sleep_until(
        std::chrono::steady_clock::time_point time, Function&& function) {
        timer_.expires_at(time);
        timer_.async_wait([function_moved = std::forward<Function>(function)](
                              const asio::error_code& error) mutable {
            if (!error) {
                function_moved();
            }
        });
    }

    /*!
     * \brief Asynchronously sleep for a duration.
     *
     * \tparam Function Type of the function.
     * \param[in] duration Duration.
     * \param[in] function Function called when the time come.
     */
    template <typename Function>
    void async_sleep_for(
        std::chrono::steady_clock::duration duration, Function&& function) {
        timer_.expires_after(duration);
        timer_.async_wait([function_moved = std::forward<Function>(function)](
                              const asio::error_code& error) mutable {
            if (!error) {
                function_moved();
            }
        });
    }

private:
    //! Timer in asio library.
    asio::steady_timer timer_;
};

}  // namespace msgpack_rpc::executors
