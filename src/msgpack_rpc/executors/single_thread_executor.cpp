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
 * \brief Definition of SingleThreadExecutor class.
 */
#include <exception>
#include <memory>
#include <utility>

#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/i_single_thread_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Class of executors runs in a single thread.
 */
class SingleThreadExecutor final : public ISingleThreadExecutor {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     */
    explicit SingleThreadExecutor(std::shared_ptr<logging::Logger> logger)
        : logger_(std::move(logger)) {}

    //! \copydoc msgpack_rpc::executors::ISingleThreadExecutor::run
    void run() override {
        try {
            MSGPACK_RPC_TRACE(logger_, "Start an executor.");
            context_.run();
            MSGPACK_RPC_TRACE(logger_, "Executor run stopped normally.");
        } catch (const std::exception& e) {
            MSGPACK_RPC_CRITICAL(
                logger_, "Executor stopped due to an exception: {}", e.what());
            throw;
        }
    }

    //! \copydoc msgpack_rpc::executors::IExecutor::context
    AsioContextType& context(OperationType type) noexcept override {
        (void)type;
        return context_;
    }

private:
    //! Context.
    AsioContextType context_{1};

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

std::shared_ptr<ISingleThreadExecutor> create_single_thread_executor(
    std::shared_ptr<logging::Logger> logger) {
    return std::make_shared<SingleThreadExecutor>(std::move(logger));
}

}  // namespace msgpack_rpc::executors
