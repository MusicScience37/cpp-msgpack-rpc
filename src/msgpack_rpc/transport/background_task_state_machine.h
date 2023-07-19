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
 * \brief Definition of BackgroundTaskStateMachine class.
 */
#pragma once

#include <atomic>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of state machines of background tasks in connections and
 * acceptors.
 */
class BackgroundTaskStateMachine {
public:
    //! Enumeration of states.
    enum class State {
        //! Initial state.
        INIT,

        //! Starting.
        STARTING,

        //! Processing.
        PROCESSING,

        //! Stopped.
        STOPPED
    };

    //! Constructor.
    BackgroundTaskStateMachine() = default;

    /*!
     * \brief Handle the condition that starting processing is requested.
     */
    void handle_start_request() {
        auto expected_state = State::INIT;
        const auto next_state = State::STARTING;
        if (!state_.compare_exchange_strong(
                expected_state, next_state, std::memory_order_relaxed)) {
            throw MsgpackRPCException(
                StatusCode::PRECONDITION_NOT_MET, "Already started.");
        }
    }

    /*!
     * \brief Handle the condition that processing is started.
     */
    void handle_processing_started() {
        state_.store(State::PROCESSING, std::memory_order_release);
    }

    /*!
     * \brief Handle the condition that stopping processing is requested.
     *
     * \retval true Not stopped yet, so stop is required.
     * \retval false Already stopped, so stop is not required.
     */
    bool handle_stop_requested() {
        return state_.exchange(State::STOPPED, std::memory_order_relaxed) !=
            State::STOPPED;
    }

    /*!
     * \brief Handle the condition that processing has been stopped.
     */
    void handle_processing_stopped() {
        state_.store(State::STOPPED, std::memory_order_relaxed);
    }

    /*!
     * \brief Check whether processing.
     *
     * \retval true Currently processing.
     * \retval false Currently not processing.
     */
    [[nodiscard]] bool is_processing() const noexcept {
        return state_.load(std::memory_order_acquire) == State::PROCESSING;
    }

private:
    //! Whether the process of this connection is started.
    std::atomic<State> state_{State::INIT};
};

}  // namespace msgpack_rpc::transport
