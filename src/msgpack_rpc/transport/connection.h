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
 * \brief Definition of Connection class.
 */
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/post.hpp>
#include <asio/write.hpp>
#include <fmt/format.h>

#include "msgpack_rpc/addresses/i_address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/buffer_view.h"
#include "msgpack_rpc/messages/message_parser.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/messages/serialized_message.h"
#include "msgpack_rpc/transport/background_task_state_machine.h"
#include "msgpack_rpc/transport/connection_list.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Class of connections.
 *
 * \param AsioSocketType Type of sockets in asio library.
 * \param ConcreteAddressType Type of concrete addresses.
 */
template <typename AsioSocketType, typename ConcreteAddressType>
class Connection : public IConnection,
                   public std::enable_shared_from_this<
                       Connection<AsioSocketType, ConcreteAddressType>> {
public:
    //! Type of sockets in asio library.
    using AsioSocket = AsioSocketType;

    //! Type of concrete addresses.
    using ConcreteAddress = ConcreteAddressType;

    /*!
     * \brief Constructor.
     *
     * \param[in] socket Socket.
     * \param[in] message_parser_config Configuration of the parser of messages.
     * \param[in] logger Logger.
     * \param[in] connection_list List of connections.
     */
    Connection(AsioSocket&& socket,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger,
        const std::shared_ptr<ConnectionList<Connection>>& connection_list =
            nullptr)
        : socket_(std::move(socket)),
          message_parser_(message_parser_config),
          local_address_(socket_.local_endpoint()),
          remote_address_(socket_.remote_endpoint()),
          log_name_(fmt::format("Connection(local={}, remote={})",
              local_address_, remote_address_)),
          logger_(std::move(logger)),
          connection_list_(connection_list) {}

    Connection(const Connection&) = delete;
    Connection(Connection&&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection& operator=(Connection&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~Connection() override {
        const auto connection_list = connection_list_.lock();
        if (connection_list) {
            connection_list->remove(this);
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

        asio::post(socket_.get_executor(),
            [self = this->shared_from_this()] { self->async_read_next(); });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::async_send
    void async_send(const messages::SerializedMessage& message) override {
        if (!state_machine_.is_processing()) {
            MSGPACK_RPC_TRACE(logger_, "Not processing now.");
            return;
        }
        asio::post(socket_.get_executor(),
            [self = this->shared_from_this(), message]() {
                self->async_send_in_thread(message);
            });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::async_close
    void async_close() override {
        asio::post(socket_.get_executor(), [self = this->shared_from_this()]() {
            self->close_in_thread(Status());
        });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::local_address
    [[nodiscard]] const addresses::IAddress& local_address()
        const noexcept override {
        return local_address_;
    }

    //! \copydoc msgpack_rpc::transport::IConnection::remote_address
    [[nodiscard]] const addresses::IAddress& remote_address()
        const noexcept override {
        return remote_address_;
    }

private:
    /*!
     * \brief Asynchronously read next bytes.
     */
    void async_read_next() {
        const auto buffer = message_parser_.prepare_buffer();
        socket_.async_read_some(asio::buffer(buffer.data(), buffer.size()),
            [self = this->shared_from_this()](const asio::error_code& error,
                std::size_t size) { self->process_read_bytes(error, size); });
        MSGPACK_RPC_TRACE(logger_, "({}) Reading next bytes.", log_name_);
    }

    /*!
     * \brief Process read bytes.
     *
     * \param[in] error Error.
     * \param[in] size Number of bytes read to the buffer.
     */
    void process_read_bytes(const asio::error_code& error, std::size_t size) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            if (error == asio::error::eof ||
                error == asio::error::connection_reset ||
                error == asio::error::broken_pipe) {
                MSGPACK_RPC_TRACE(
                    logger_, "({}) Connection closed by peer.", log_name_);
                state_machine_.handle_processing_stopped();
                on_closed_(Status());
                return;
            }
            const auto message = fmt::format(
                "Error occurred when receiving data: {}", error.message());
            MSGPACK_RPC_ERROR(logger_, "({}) {}", log_name_, message);
            throw MsgpackRPCException(StatusCode::UNEXPECTED_ERROR, message);
        }

        MSGPACK_RPC_TRACE(logger_, "({}) Read {} bytes.", log_name_, size);
        message_parser_.consumed(size);

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
                    logger_, "({}) Received a message.", log_name_, size);
                on_received_(std::move(*message));
                message.reset();
            } else {
                MSGPACK_RPC_TRACE(logger_,
                    "({}) More bytes are needed to parse a message.",
                    log_name_);
                break;
            }
        }

        if (!state_machine_.is_processing()) {
            return;
        }
        async_read_next();
    }

    /*!
     * \brief Asynchronously send a message in this thread.
     *
     * \param[in] message Message.
     */
    void async_send_in_thread(const messages::SerializedMessage& message) {
        asio::async_write(socket_,
            asio::const_buffer(message.data(), message.size()),
            [self = this->shared_from_this(), message](
                const asio::error_code& error, std::size_t /*size*/) {
                self->on_sent(error, message.size());
            });
        MSGPACK_RPC_TRACE(
            logger_, "({}) Sending {} bytes.", log_name_, message.size());
    }

    /*!
     * \brief Handle the result of send operation.
     *
     * \param[in] error Error.
     * \param[in] size Message size.
     */
    void on_sent(const asio::error_code& error, std::size_t size) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            if (error == asio::error::eof ||
                error == asio::error::connection_reset ||
                error == asio::error::broken_pipe) {
                MSGPACK_RPC_TRACE(
                    logger_, "({}) Connection closed by peer.", log_name_);
                state_machine_.handle_processing_stopped();
                on_closed_(Status());
                return;
            }
            const auto message = fmt::format(
                "Error occurred when sending data: {}", error.message());
            MSGPACK_RPC_ERROR(logger_, "({}) {}", log_name_, message);
            throw MsgpackRPCException(StatusCode::UNEXPECTED_ERROR, message);
        }

        MSGPACK_RPC_TRACE(logger_, "({}) Sent {} bytes.", log_name_, size);
        on_sent_();
    }

    /*!
     * \brief Close this connection in this thread.
     *
     * \param[in] status Status.
     */
    void close_in_thread(const Status& status) {
        if (!state_machine_.handle_stop_requested()) {
            return;
        }
        socket_.cancel();
        socket_.shutdown(AsioSocket::shutdown_both);
        socket_.close();
        on_closed_(status);
        MSGPACK_RPC_TRACE(logger_, "({}) Closed this connection.", log_name_);
    }

    //! Socket.
    AsioSocket socket_;

    //! Callback function when a message is received.
    MessageReceivedCallback on_received_{};

    //! Callback function when a message is sent.
    MessageSentCallback on_sent_{};

    //! Callback function when this connection is closed.
    ConnectionClosedCallback on_closed_{};

    //! Parser of messages.
    messages::MessageParser message_parser_;

    //! Address of the local endpoint.
    ConcreteAddress local_address_;

    //! Address of the remote endpoint.
    ConcreteAddress remote_address_;

    //! Name of the connection for logs.
    std::string log_name_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! State machine.
    BackgroundTaskStateMachine state_machine_{};

    //! List of connections.
    std::weak_ptr<ConnectionList<Connection>> connection_list_;
};

}  // namespace msgpack_rpc::transport
