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
 * \brief Definition of StopSignalHandler class.
 */
#pragma once

#include <memory>

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>

#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc::servers {

/*!
 * \brief Class to handle Linux signals to stop a server.
 */
class StopSignalHandler {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     */
    explicit StopSignalHandler(std::shared_ptr<logging::Logger> logger)
        : context_(1),
          signal_set_(context_, SIGINT, SIGTERM),
          logger_(std::move(logger)) {}

    /*!
     * \brief Wait for a signal.
     */
    void wait() {
        signal_set_.async_wait(
            [this](const asio::error_code& error, int signal_number) {
                if (!error) {
                    MSGPACK_RPC_DEBUG(
                        logger_, "Received signal {}.", signal_number);
                }
            });
        context_.run();
    }

    /*!
     * \brief Stop waiting.
     */
    void stop() { context_.stop(); }

private:
    //! Context of asio library.
    asio::io_context context_;

    //! Object to wait signals.
    asio::signal_set signal_set_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

}  // namespace msgpack_rpc::servers
