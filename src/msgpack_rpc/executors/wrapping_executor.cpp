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
 * \brief Definition of WrappingExecutor class.
 */
#include <memory>
#include <utility>

#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/executors/i_async_executor.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Class to wrap existing executors.
 *
 * \note Resulting wrapper won't call start, stop, run functions of the given
 * executor.
 */
class WrappingExecutor final : public IAsyncExecutor {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] executor An existing executor.
     */
    explicit WrappingExecutor(std::shared_ptr<IExecutor> executor)
        : executor_(std::move(executor)) {}

    //! \copydoc msgpack_rpc::executors::IExecutor::run
    void run() override {
        // No operation.
    }

    //! \copydoc msgpack_rpc::executors::IExecutor::run_until_interruption
    void run_until_interruption() override {
        // No operation.
    }

    //! \copydoc msgpack_rpc::executors::IExecutor::interrupt
    void interrupt() override {
        // No operation.
    }

    //! \copydoc msgpack_rpc::executors::IExecutor::context
    AsioContextType& context(OperationType type) noexcept override {
        return executor_->context(type);
    }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::start
    void start() override {
        // No operation.
    }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::stop
    void stop() override {
        // No operation.
    }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::last_exception
    [[nodiscard]] std::exception_ptr last_exception() override {
        return nullptr;
    }

private:
    //! Executor.
    std::shared_ptr<IExecutor> executor_;
};

std::shared_ptr<IAsyncExecutor> wrap_executor(
    std::shared_ptr<IExecutor> executor) {
    return std::make_shared<WrappingExecutor>(std::move(executor));
}

}  // namespace msgpack_rpc::executors
