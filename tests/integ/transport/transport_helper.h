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
 * \brief Definition of helper classes for tests of transport.
 */
#pragma once

#include <memory>
#include <utility>

#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/executors/asio_context_type.h"
#include "msgpack_rpc/executors/i_executor.h"
#include "msgpack_rpc/executors/operation_type.h"
#include "msgpack_rpc/messages/parsed_message.h"
#include "msgpack_rpc/transport/i_acceptor.h"
#include "msgpack_rpc/transport/i_connection.h"
#include "trompeloeil_catch2.h"

class TestExecutor final : public msgpack_rpc::executors::IExecutor {
public:
    explicit TestExecutor(
        std::shared_ptr<msgpack_rpc::executors::IExecutor> executor)
        : executor_(std::move(executor)) {}

    void run() override { executor_->run(); }

    void run_until_interruption() override {
        executor_->run_until_interruption();
    }

    void stop() override { executor_->stop(); }

    msgpack_rpc::executors::AsioContextType& context(
        msgpack_rpc::executors::OperationType type) noexcept override {
        on_context(type);
        return executor_->context(type);
    }

    MAKE_MOCK1(on_context, void(msgpack_rpc::executors::OperationType));

private:
    std::shared_ptr<msgpack_rpc::executors::IExecutor> executor_;
};

class AcceptorCallbacks
    : public std::enable_shared_from_this<AcceptorCallbacks> {
public:
    MAKE_MOCK1(on_connection,
        void(const std::shared_ptr<msgpack_rpc::transport::IConnection>&));

    void apply_to(
        const std::shared_ptr<msgpack_rpc::transport::IAcceptor>& acceptor) {
        acceptor->start(
            [self = this->shared_from_this()](
                const std::shared_ptr<msgpack_rpc::transport::IConnection>&
                    connection) { self->on_connection(connection); });
    }
};

class ConnectionCallbacks
    : public std::enable_shared_from_this<ConnectionCallbacks> {
public:
    MAKE_MOCK1(on_received, void(msgpack_rpc::messages::ParsedMessage));
    MAKE_MOCK0(on_sent, void());
    MAKE_MOCK1(on_closed, void(msgpack_rpc::Status));

    void apply_to(const std::shared_ptr<msgpack_rpc::transport::IConnection>&
            connection) {
        connection->start(
            [self = this->shared_from_this()](
                msgpack_rpc::messages::ParsedMessage message) {
                self->on_received(std::move(message));
            },
            [self = this->shared_from_this()] { self->on_sent(); },
            [self = this->shared_from_this()](msgpack_rpc::Status status) {
                self->on_closed(std::move(status));
            });
    }
};
