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
#include <exception>
#include <functional>
#include <memory>
#include <utility>

#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/executors/wrap_executor.h"

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

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::on_exception
    void on_exception(
        std::function<void(std::exception_ptr)> exception_callback) override {
        // No operation.
        (void)exception_callback;
    }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::is_running
    [[nodiscard]] bool is_running() override { return true; }

private:
    //! Executor.
    std::shared_ptr<IExecutor> executor_;
};

std::shared_ptr<IAsyncExecutor> wrap_executor(
    std::shared_ptr<IExecutor> executor) {
    return std::make_shared<WrappingExecutor>(std::move(executor));
}

}  // namespace msgpack_rpc::executors
