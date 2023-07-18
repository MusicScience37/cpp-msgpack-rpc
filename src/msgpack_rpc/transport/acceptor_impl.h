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
 * \brief Definition of AcceptorImpl class.
 */
#pragma once

#include <functional>
#include <memory>
#include <utility>

#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <fmt/ostream.h>

#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/stream_connection_impl.h"

namespace msgpack_rpc::transport {

/*!
 * \brief Internal class of acceptors.
 */
class AcceptorImpl : public std::enable_shared_from_this<AcceptorImpl> {
public:
    //! Protocol type.
    using Protocol = asio::ip::tcp;  // TODO Change to template when another
                                     // protocol is implemented.

    //! Type of connections.
    using ConnectionImplType = StreamConnectionImpl;

    //! Type of callback functions called when a connection is accepted.
    using ConnectionCallback =
        std::function<void(std::shared_ptr<ConnectionImplType>)>;

    /*!
     * \brief Constructor.
     *
     * \param[in] context Context in asio library.
     * \param[in] local_address Local address in asio library.
     * \param[in] message_parser_config Configuration of the parser of messages.
     * \param[in] logger Logger.
     */
    AcceptorImpl(executors::AsioContextType& context,
        const Protocol::endpoint& local_address,
        const config::MessageParserConfig& message_parser_config,
        std::shared_ptr<logging::Logger> logger)
        : context_(context),
          acceptor_(context, local_address),
          message_parser_config_(message_parser_config),
          logger_(std::move(logger)) {}

    /*!
     * \brief Access to the acceptor.
     *
     * \return Acceptor.
     */
    Protocol::acceptor& acceptor() noexcept { return acceptor_; }

    /*!
     * \brief Start process of this acceptor.
     *
     * \param[in] on_connection Callback function called when a connection is
     * accepted.
     * \param[in] log_name Name of the connection for logs.
     */
    void start(ConnectionCallback on_connection, std::string log_name) {
        if (!change_state_if(State::INIT, State::STARTING)) {
            throw MsgpackRPCException(StatusCode::PRECONDITION_NOT_MET,
                "This acceptor is already started.");
        }
        // Only one thread can enter here.

        on_connection_ = std::move(on_connection);
        log_name_ = std::move(log_name);
        asio::post(acceptor_.get_executor(),
            [self = this->shared_from_this()] { self->async_accept_next(); });

        change_state_to(State::PROCESSING);
    }

    /*!
     * \brief Asynchronously stop this acceptor.
     */
    void async_stop() {
        asio::post(acceptor_.get_executor(),
            [self = this->shared_from_this()]() { self->stop_in_thread(); });
    }

private:
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

    /*!
     * \brief Asynchronously accept a connection.
     */
    void async_accept_next() {
        const auto connection = std::make_shared<ConnectionImplType>(
            context_, message_parser_config_, logger_);
        acceptor_.async_accept(connection->socket(),
            [self = this->shared_from_this(), connection](
                const asio::error_code& error) {
                self->on_accept(error, connection);
            });
    }

    /*!
     * \brief Handle the result of accept operation.
     *
     * \param[in] error Error.
     */
    void on_accept(const asio::error_code& error,
        std::shared_ptr<ConnectionImplType> connection) {
        if (error) {
            if (error == asio::error::operation_aborted) {
                return;
            }
            const auto message =
                fmt::format("Error occurred when accepting a connection: {}",
                    error.message());
            MSGPACK_RPC_ERROR(logger_, "({}) {}", log_name_, message);
            throw MsgpackRPCException(StatusCode::UNEXPECTED_ERROR, message);
        }

        MSGPACK_RPC_TRACE(logger_, "({}) Accepted a connection from {}.",
            log_name_, fmt::streamed(connection->socket().remote_endpoint()));
        on_connection_(std::move(connection));

        if (current_state() != State::PROCESSING) {
            return;
        }
        async_accept_next();
    }

    /*!
     * \brief Stop this connection in this thread.
     */
    void stop_in_thread() {
        if (state_.exchange(State::STOPPED) == State::STOPPED) {
            return;
        }
        acceptor_.cancel();
        acceptor_.close();
        MSGPACK_RPC_TRACE(logger_, "({}) Stopped this acceptor.", log_name_);
    }

    /*!
     * \brief Change the state of this connection if the current state matches
     * to an expected state.
     *
     * \param[in] expected_state Expected state.
     * \param[in] next_state Next state.
     * \return Whether the current state matched the expected state.
     */
    bool change_state_if(State expected_state, State next_state) {
        return state_.compare_exchange_strong(
            expected_state, next_state, std::memory_order_acquire);
    }

    /*!
     * \brief Change the state of this connection.
     *
     * \param[in] next_state Next state.
     */
    void change_state_to(State next_state) {
        state_.store(next_state, std::memory_order_release);
    }

    /*!
     * \brief Get the current state of this connection.
     *
     * \return Current state.
     */
    [[nodiscard]] State current_state() {
        return state_.load(std::memory_order_relaxed);
    }

    //! Context.
    executors::AsioContextType& context_;

    //! Acceptor.
    Protocol::acceptor acceptor_;

    //! Callback function called when a connection is accepted.
    ConnectionCallback on_connection_{};

    //! Configuration of parsers of messages.
    config::MessageParserConfig message_parser_config_;

    //! Name of the connection for logs.
    std::string log_name_{};

    //! Logger.
    std::shared_ptr<logging::Logger> logger_;

    //! Whether the process of this connection is started.
    std::atomic<State> state_{State::INIT};
};

}  // namespace msgpack_rpc::transport
