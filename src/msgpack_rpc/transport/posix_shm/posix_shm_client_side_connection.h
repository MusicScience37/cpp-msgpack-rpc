/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of PosixShmClientSideConnection class.
 */
#pragma once

#include <chrono>

#include "msgpack_rpc/config.h"

#if MSGPACK_RPC_HAS_POSIX_SHM

#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <asio/post.hpp>
#include <boost/memory_order.hpp>
#include <fmt/format.h>

#include "msgpack_rpc/addresses/posix_shared_memory_address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/impl/msgpack_rpc_export.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_parser.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/transport/background_task_state_machine.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "msgpack_rpc/transport/posix_shm/changes_count.h"
#include "msgpack_rpc/transport/posix_shm/client_id.h"
#include "msgpack_rpc/transport/posix_shm/client_memory_address_calculator.h"
#include "msgpack_rpc/transport/posix_shm/clint_state.h"
#include "msgpack_rpc/transport/posix_shm/posix_shared_memory.h"
#include "msgpack_rpc/transport/posix_shm/server_event_queue.h"
#include "msgpack_rpc/transport/posix_shm/server_memory_address_calculator.h"
#include "msgpack_rpc/transport/posix_shm/server_state.h"
#include "msgpack_rpc/transport/posix_shm/shm_stream_reader.h"
#include "msgpack_rpc/transport/posix_shm/shm_stream_writer.h"

namespace msgpack_rpc::transport::posix_shm {

/*!
 * \brief Class of connections on the client side in POSIX shared memory.
 */
class MSGPACK_RPC_EXPORT PosixShmClientSideConnection final
    : public IConnection,
      public std::enable_shared_from_this<PosixShmClientSideConnection> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] client_shared_memory Shared memory of the client.
     * \param[in] client_memory_address_calculator Calculator of addresses on
     * shared memory of the client.
     * \param[in] server_memory_address_calculator Calculator of addresses on
     * shared memory of the server.
     * \param[in] client_id Client ID.
     * \param[in] message_parser_config Configuration of parsers of messages.
     * \param[in] shared_memory_name Name of the shared memory.
     * \param[in] logger Logger.
     * \param[in] context Context to execute callbacks.
     */
    PosixShmClientSideConnection(PosixSharedMemory&& client_shared_memory,
        const ClientMemoryAddressCalculator& client_memory_address_calculator,
        const ServerMemoryAddressCalculator& server_memory_address_calculator,
        ClientID client_id,
        const config::MessageParserConfig& message_parser_config,
        std::string_view shared_memory_name,
        std::shared_ptr<logging::Logger> logger,
        executors::AsioContextType& context)
        : client_shared_memory_(std::move(client_shared_memory)),
          client_changes_count_(
              client_memory_address_calculator.changes_count()),
          client_state_(client_memory_address_calculator.client_state()),
          client_to_server_stream_writer_(
              client_memory_address_calculator
                  .client_to_server_stream_writer()),
          server_to_client_stream_reader_(
              client_memory_address_calculator
                  .server_to_client_stream_reader()),
          server_state_(server_memory_address_calculator.server_state()),
          server_event_queue_(
              server_memory_address_calculator.server_event_queue()),
          client_id_(client_id),
          message_parser_(message_parser_config),
          address_(static_cast<std::string>(shared_memory_name)),
          log_name_(fmt::format("Connection(uri=shm://{}, client_id={})",
              shared_memory_name, client_id_)),
          logger_(std::move(logger)),
          context_(context) {}

    PosixShmClientSideConnection(const PosixShmClientSideConnection&) = delete;
    PosixShmClientSideConnection(PosixShmClientSideConnection&&) = delete;
    PosixShmClientSideConnection& operator=(
        const PosixShmClientSideConnection&) = delete;
    PosixShmClientSideConnection& operator=(
        PosixShmClientSideConnection&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~PosixShmClientSideConnection() override {
        if (read_thread_.joinable()) {
            client_state_->store(ClientState::DISCONNECTED);
            client_changes_count_.increment();
            read_thread_.join();
        }
    }

    //! \copydoc msgpack_rpc::transport::IConnection::start
    void start(MessageReceivedCallback on_received, MessageSentCallback on_sent,
        ConnectionClosedCallback on_closed) override {
        state_machine_.handle_start_request();
        // Only one thread can enter here.

        on_received_ = std::move(on_received);
        on_sent_ = std::move(on_sent);
        on_closed_ = std::move(on_closed);

        state_machine_.handle_processing_started();

        client_state_->store(ClientState::RUNNING, boost::memory_order_relaxed);

        read_thread_ = std::thread{[this] { read_messages_in_thread(); }};
    }

    //! \copydoc msgpack_rpc::transport::IConnection::async_send
    void async_send(const messages::SerializedMessage& message) override {
        if (!state_machine_.is_processing()) {
            MSGPACK_RPC_TRACE(logger_, "Not processing now.");
            return;
        }
        asio::post(context_, [self = this->shared_from_this(), message]() {
            self->async_send_in_thread(message);
        });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::async_close
    void async_close() override {
        state_machine_.handle_processing_stopped();
        client_state_->store(
            ClientState::DISCONNECTED, boost::memory_order_relaxed);
        client_changes_count_.increment();
        asio::post(context_, [self = this->shared_from_this()] {
            self->notify_event(ServerEventType::CLIENT_DESTROYED);
            self->notify_closed(Status());
        });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::local_address
    [[nodiscard]] const addresses::IAddress& local_address()
        const noexcept override {
        return address_;
    }

    //! \copydoc msgpack_rpc::transport::IConnection::remote_address
    [[nodiscard]] const addresses::IAddress& remote_address()
        const noexcept override {
        return address_;
    }

private:
    /*!
     * \brief Read messages.
     */
    void read_messages_in_thread() {
        const auto thread_id = create_thread_id_string();
        MSGPACK_RPC_TRACE(logger_,
            "({}) Start a thread for shared memory transport {}.", log_name_,
            thread_id);

        try {
            while (true) {
                const auto client_changes_count = client_changes_count_.count();

                if (client_state_->load(boost::memory_order_relaxed) !=
                        ClientState::RUNNING ||
                    server_state_->load(boost::memory_order_relaxed) !=
                        ServerState::RUNNING) {
                    state_machine_.handle_processing_stopped();
                    notify_closed(Status());
                    break;
                }

                read_messages_impl();

                // TODO configuration for timeout
                constexpr auto timeout = std::chrono::milliseconds{100};
                client_changes_count_.wait_for_change_from(
                    client_changes_count, timeout);
            }
        } catch (const std::exception& e) {
            client_state_->store(
                ClientState::DISCONNECTED, boost::memory_order_relaxed);
            MSGPACK_RPC_CRITICAL(logger_,
                "({}) Unexpected exception in a thread for shared memory "
                "transport: {}",
                log_name_, e.what());
        }

        MSGPACK_RPC_TRACE(logger_,
            "({}) Finish a thread for shared memory transport {}.", log_name_,
            thread_id);
    }

    /*!
     * \brief Read messages.
     */
    void read_messages_impl() {
        auto buffer = message_parser_.prepare_buffer();
        const std::size_t read_bytes =
            server_to_client_stream_reader_.read_some(
                buffer.data(), buffer.size());
        if (read_bytes == 0) {
            return;
        }
        MSGPACK_RPC_TRACE(
            logger_, "({}) Read {} bytes.", log_name_, read_bytes);
        message_parser_.consumed(read_bytes);
        client_changes_count_.increment();
        if (!notify_event(ServerEventType::CLIENT_STATE_CHANGED)) {
            return;
        }

        while (true) {
            std::optional<messages::ParsedMessage> message;
            try {
                message = message_parser_.try_parse();
            } catch (const MsgpackRPCException& e) {
                MSGPACK_RPC_ERROR(
                    logger_, "({}) {}", log_name_, e.status().message());
                close_in_thread(e.status());
                return;
            }
            if (message) {
                MSGPACK_RPC_TRACE(
                    logger_, "({}) Received a message.", log_name_);
                on_received_(std::move(*message));
                message.reset();
            } else {
                MSGPACK_RPC_TRACE(logger_,
                    "({}) More bytes are needed to parse a message.",
                    log_name_);
                break;
            }
        }
    }

    /*!
     * \brief Asynchronously send a message in this thread.
     *
     * \param[in] message Message.
     */
    void async_send_in_thread(const messages::SerializedMessage& message) {
        std::size_t total_written_bytes = 0;
        while (total_written_bytes < message.size()) {
            const auto client_changes_count = client_changes_count_.count();

            if (client_state_->load(boost::memory_order_relaxed) !=
                    ClientState::RUNNING ||
                server_state_->load(boost::memory_order_relaxed) !=
                    ServerState::RUNNING) {
                state_machine_.handle_processing_stopped();
                notify_closed(Status());
                return;
            }

            const std::size_t written_size =
                client_to_server_stream_writer_.write_some(
                    message.data() + total_written_bytes,
                    message.size() - total_written_bytes);
            if (written_size > 0) {
                total_written_bytes += written_size;
                client_changes_count_.increment();
            } else {
                if (!notify_event(ServerEventType::CLIENT_STATE_CHANGED)) {
                    state_machine_.handle_processing_stopped();
                    notify_closed(Status());
                    return;
                }
                // TODO configuration for timeout
                constexpr auto timeout = std::chrono::milliseconds{100};
                client_changes_count_.wait_for_change_from(
                    client_changes_count, timeout);
            }
        }
        notify_event(ServerEventType::CLIENT_STATE_CHANGED);
        on_sent_();
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
     * \brief Notify that this connection is closed.
     *
     * \param[in] status Status.
     */
    void notify_closed(const Status& status) {
        if (!on_closed_called_.exchange(true)) {
            MSGPACK_RPC_TRACE(logger_, "({}) Connection is closed.", log_name_);
            on_closed_(status);
        }
    }

    /*!
     * \brief Close this connection.
     *
     * \param[in] status Status.
     */
    void close_in_thread(const Status& status) {
        state_machine_.handle_processing_stopped();
        client_state_->store(
            ClientState::DISCONNECTED, boost::memory_order_relaxed);
        client_changes_count_.increment();
        notify_event(ServerEventType::CLIENT_DESTROYED);
        notify_closed(status);
    }

    /*!
     * \brief Notify an event to the server.
     *
     * \param[in] type Type of the event.
     * \return Whether the event is notified.
     */
    bool notify_event(ServerEventType type) {
        ServerEvent event{client_id_, type};
        while (true) {
            if (server_state_->load(boost::memory_order_relaxed) !=
                ServerState::RUNNING) {
                return false;
            }

            // TODO configuration for timeout
            constexpr auto timeout = std::chrono::milliseconds{100};
            if (server_event_queue_.push(event, timeout)) {
                return true;
            }
        }
    }

    //! Shared memory of the client.
    PosixSharedMemory client_shared_memory_;

    //! Count of changes of the client.
    ChangesCount client_changes_count_;

    //! State of the client.
    AtomicClientState* client_state_;

    //! Writer of the stream from the client to the server.
    ShmStreamWriter client_to_server_stream_writer_;

    //! Reader of the stream from the server to the client.
    ShmStreamReader server_to_client_stream_reader_;

    //! State of the server.
    AtomicServerState* server_state_;

    //! Event queue of the server.
    ServerEventQueue server_event_queue_;

    //! Client ID.
    ClientID client_id_;

    //! Callback function when a message is received.
    MessageReceivedCallback on_received_{};

    //! Callback function when a message is sent.
    MessageSentCallback on_sent_{};

    //! Callback function when this connection is closed.
    ConnectionClosedCallback on_closed_{};

    //! Parser of messages.
    messages::MessageParser message_parser_;

    //! Address of the shared memory.
    addresses::PosixSharedMemoryAddress address_;

    //! Name of the connection for logs.
    std::string log_name_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! State machine.
    BackgroundTaskStateMachine state_machine_{};

    //! Context to execute callbacks.
    executors::AsioContextType& context_;

    //! Thread to read messages.
    std::thread read_thread_{};

    //! Whether on_closed_ has been called.
    std::atomic<bool> on_closed_called_{false};
};

}  // namespace msgpack_rpc::transport::posix_shm

#endif
