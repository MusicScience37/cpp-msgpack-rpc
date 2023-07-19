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

#include <atomic>
#include <cstddef>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/post.hpp>
#include <asio/write.hpp>
#include <fmt/core.h>

#include "msgpack_rpc/addresses/tcp_address.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/messages/message_parser.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/transport/background_task_state_machine.h"
#include "msgpack_rpc/transport/i_connection.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Internal class of connections.
 */
class Connection : public IConnection,
                   public std::enable_shared_from_this<Connection> {
public:
    // TODO Change to template when another protocol is implemented.

    //! Type of sockets in asio library.
    using AsioSocket = asio::ip::tcp::socket;

    //! Type of concrete addresses.
    using ConcreteAddress = addresses::TCPAddress;

    /*!
     * \brief Constructor.
     *
     * \param[in] context Context in asio library.
     * \param[in] message_parser_config Configuration of the parser of messages.
     * \param[in] logger Logger.
     */
    Connection(AsioSocket&& socket,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger)
        : socket_(std::move(socket)),
          message_parser_(message_parser_config),
          local_address_(ConcreteAddress(socket_.local_endpoint())),
          remote_address_(ConcreteAddress(socket_.remote_endpoint())),
          log_name_(fmt::format("Connection(local={}, remote={})",
              local_address_, remote_address_)),
          logger_(std::move(logger)) {}

    /*!
     * \brief Access to the socket.
     *
     * \return Socket.
     */
    AsioSocket& socket() noexcept { return socket_; }

    //! \copydoc msgpack_rpc::transport::IConnection::start
    void start(MessageReceivedCallback on_received, MessageSentCallback on_sent,
        ConnectionClosedCallback on_closed) override {
        state_machine_.handle_start_request();
        // Only one thread can enter here.

        on_received_ = std::move(on_received);
        on_sent_ = std::move(on_sent);
        on_closed_ = std::move(on_closed);
        asio::post(socket_.get_executor(),
            [self = this->shared_from_this()] { self->async_read_next(); });

        state_machine_.handle_processing_started();
    }

    //! \copydoc msgpack_rpc::transport::IConnection::async_send
    void async_send(
        std::shared_ptr<const messages::SerializedMessage> message) override {
        if (!state_machine_.is_processing()) {
            return;
        }
        asio::post(socket_.get_executor(),
            [self = this->shared_from_this(),
                message_moved = std::move(message)]() {
                self->async_send_in_thread(message_moved);
            });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::async_close
    void async_close() override {
        asio::post(socket_.get_executor(), [self = this->shared_from_this()]() {
            self->close_in_thread(Status());
        });
    }

    //! \copydoc msgpack_rpc::transport::IConnection::local_address
    [[nodiscard]] const addresses::Address& local_address()
        const noexcept override {
        return local_address_;
    }

    //! \copydoc msgpack_rpc::transport::IConnection::remote_address
    [[nodiscard]] const addresses::Address& remote_address()
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
            if (error == asio::error::eof) {
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
    void async_send_in_thread(
        const std::shared_ptr<const messages::SerializedMessage>& message) {
        asio::async_write(socket_,
            asio::const_buffer(message->data(), message->size()),
            [self = this->shared_from_this(), message](
                const asio::error_code& error, std::size_t /*size*/) {
                self->on_sent(error, message->size());
            });
        MSGPACK_RPC_TRACE(
            logger_, "({}) Sending {} bytes.", log_name_, message->size());
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
            if (error == asio::error::eof) {
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
    addresses::Address local_address_;

    //! Address of the remote endpoint.
    addresses::Address remote_address_;

    //! Name of the connection for logs.
    std::string log_name_;

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! State machine.
    BackgroundTaskStateMachine state_machine_{};
};

}  // namespace msgpack_rpc::transport
