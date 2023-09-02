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
 * \brief Definition of GeneralExecutor class.
 */
#include <atomic>
#include <cstdlib>
#include <exception>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <asio/executor_work_guard.hpp>

#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/logging/logger.h"

namespace msgpack_rpc::executors {

/*!
 * \brief Class of general-purpose executors.
 */
class GeneralExecutor final : public IAsyncExecutor {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] logger Logger.
     * \param[in] config Configuration.
     */
    GeneralExecutor(std::shared_ptr<logging::Logger> logger,
        const config::ExecutorConfig& config)
        : transport_context_thread_pairs_(config.num_transport_threads()),
          callbacks_context_thread_pairs_(config.num_callback_threads()),
          logger_(std::move(logger)) {}

    GeneralExecutor(const GeneralExecutor&) = delete;
    GeneralExecutor(GeneralExecutor&&) = delete;
    GeneralExecutor& operator=(const GeneralExecutor&) = delete;
    GeneralExecutor& operator=(GeneralExecutor&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~GeneralExecutor() override { stop(); }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::start
    void start() override {
        if (is_started_.exchange(true)) {
            throw MsgpackRPCException(StatusCode::PRECONDITION_NOT_MET,
                "An executor must not be run multiple times.");
        }

        MSGPACK_RPC_TRACE(logger_, "Start an executor.");
        start_threads();
    }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::stop
    void stop() override {
        if (is_stopped_.exchange(true)) {
            // Already stopped.
            return;
        }

        stop_threads();

        // Remove objects
        transport_context_thread_pairs_.clear();
        callbacks_context_thread_pairs_.clear();

        MSGPACK_RPC_TRACE(logger_, "Executor run stopped.");
    }

    //! \copydoc msgpack_rpc::executors::IExecutor::context
    AsioContextType& context(OperationType type) noexcept override {
        switch (type) {
        case OperationType::TRANSPORT:
            return transport_context_thread_pairs_
                [get_context_index(transport_context_index_,
                     transport_context_thread_pairs_.size())]
                    .context;
        case OperationType::CALLBACK:
            return callbacks_context_thread_pairs_
                [get_context_index(callback_context_index_,
                     callbacks_context_thread_pairs_.size())]
                    .context;
        }
        // This line won't be executed without a bug.
        std::abort();
    }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::last_exception
    [[nodiscard]] std::exception_ptr last_exception() override {
        std::unique_lock<std::mutex> lock(exception_in_thread_mutex_);
        return exception_in_thread_;
    }

    //! \copydoc msgpack_rpc::executors::IAsyncExecutor::is_running
    [[nodiscard]] bool is_running() override {
        std::unique_lock<std::mutex> lock(exception_in_thread_mutex_);
        if (exception_in_thread_) {
            return false;
        }
        return is_started_.load(std::memory_order_relaxed) &&
            !is_stopped_.load(std::memory_order_relaxed);
    }

private:
    /*!
     * \brief Start threads.
     */
    void start_threads() {
        try {
            for (auto& context_thread_pair : transport_context_thread_pairs_) {
                context_thread_pair.thread =
                    std::thread{[this, &context_thread_pair] {
                        run_in_thread(context_thread_pair.context);
                    }};
            }
            for (auto& context_thread_pair : callbacks_context_thread_pairs_) {
                context_thread_pair.thread =
                    std::thread{[this, &context_thread_pair] {
                        run_in_thread(context_thread_pair.context);
                    }};
            }
        } catch (...) {
            stop_threads();
            throw;
        }
    }

    /*!
     * \brief Stop threads.
     */
    void stop_threads() {
        async_stop_threads_gently();
        for (auto& context_thread_pair : transport_context_thread_pairs_) {
            if (context_thread_pair.thread.joinable()) {
                context_thread_pair.thread.join();
            }
        }
        for (auto& context_thread_pair : callbacks_context_thread_pairs_) {
            if (context_thread_pair.thread.joinable()) {
                context_thread_pair.thread.join();
            }
        }
    }

    /*!
     * \brief Notify threads to stop operations.
     */
    void interrupt_threads() {
        for (auto& context_thread_pair : transport_context_thread_pairs_) {
            context_thread_pair.context.stop();
        }
        for (auto& context_thread_pair : callbacks_context_thread_pairs_) {
            context_thread_pair.context.stop();
        }
    }

    /*!
     * \brief Notify threads to stop operations gently.
     */
    void async_stop_threads_gently() {
        for (auto& context_thread_pair : transport_context_thread_pairs_) {
            context_thread_pair.work_guard.reset();
        }
        for (auto& context_thread_pair : callbacks_context_thread_pairs_) {
            context_thread_pair.work_guard.reset();
        }
    }

    /*!
     * \brief Run operations in a thread.
     *
     * \param[in] context Context for this thread.
     */
    void run_in_thread(AsioContextType& context) {
        const auto thread_id = create_thread_id_string();
        MSGPACK_RPC_TRACE(logger_, "Start an executor thread {}.", thread_id);
        try {
            context.run();
        } catch (const std::exception& e) {
            MSGPACK_RPC_CRITICAL(logger_,
                "Executor stops due to an exception thrown in thread {}: {}",
                thread_id, e.what());
            {
                std::unique_lock<std::mutex> lock(exception_in_thread_mutex_);
                exception_in_thread_ = std::current_exception();
            }
            interrupt_threads();
        }
        MSGPACK_RPC_TRACE(logger_, "Finish an executor thread {}.", thread_id);
    }

    /*!
     * \brief Create a string of the current thread ID.
     *
     * \return String of the current thread ID.
     */
    static std::string create_thread_id_string() {
        std::ostringstream stream;
        stream << std::this_thread::get_id();
        return stream.str();
    }

    /*!
     * \brief Get the index of context to use.
     *
     * \param[in] index Atomic variable of the index of context.
     * \param[in] size Number of context.
     * \return Index of context to use.
     */
    static std::size_t get_context_index(
        std::atomic<std::size_t>& index, std::size_t size) {
        // TODO Should I consider overflow?
        return index.fetch_add(1, std::memory_order_relaxed) % size;
    }

    //! Pair of context and its thread.
    struct ContextThreadPair {
    public:
        //! Context.
        AsioContextType context;

        //! Work guard.
        asio::executor_work_guard<AsioContextType::executor_type> work_guard;

        //! Thread.
        std::thread thread;

        //! Constructor.
        ContextThreadPair()
            : context(1), work_guard(asio::make_work_guard(context)) {}
    };

    //! List of pairs of context and its thread for transport.
    std::vector<ContextThreadPair> transport_context_thread_pairs_;

    //! List of pairs of context and its thread for callbacks.
    std::vector<ContextThreadPair> callbacks_context_thread_pairs_;

    //! Index of context to use for transport.
    std::atomic<std::size_t> transport_context_index_{0};

    //! Index of context to use for callbacks.
    std::atomic<std::size_t> callback_context_index_{0};

    //! Whether this executor has been started.
    std::atomic<bool> is_started_{false};

    //! Whether this executor has been stopped.
    std::atomic<bool> is_stopped_{false};

    //! Exception thrown in threads.
    std::exception_ptr exception_in_thread_{};

    //! Mutex of exception_in_thread_.
    std::mutex exception_in_thread_mutex_{};

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;
};

std::shared_ptr<IAsyncExecutor> create_executor(
    std::shared_ptr<logging::Logger> logger,
    const config::ExecutorConfig& config) {
    return std::make_shared<GeneralExecutor>(std::move(logger), config);
}

}  // namespace msgpack_rpc::executors
