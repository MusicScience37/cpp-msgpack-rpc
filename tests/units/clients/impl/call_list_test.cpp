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
 * \brief Test of CallList class.
 */
#include "msgpack_rpc/clients/impl/call_list.h"

#include <chrono>
#include <string>
#include <tuple>

#include <catch2/catch_test_macros.hpp>

#include "../../create_test_logger.h"
#include "msgpack_rpc/clients/impl/call.h"
#include "msgpack_rpc/clients/impl/call_future_impl.h"
#include "msgpack_rpc/clients/impl/parameters_serializer.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/executors/i_async_executor.h"
#include "msgpack_rpc/messages/message_id.h"
#include "msgpack_rpc/messages/method_name_view.h"
#include "msgpack_rpc/messages/parsed_parameters.h"
#include "msgpack_rpc/messages/parsed_request.h"
#include "msgpack_rpc_test/create_parsed_messages.h"
#include "msgpack_rpc_test/parse_messages.h"

TEST_CASE("msgpack_rpc::clients::impl::CallList") {
    using msgpack_rpc::MsgpackRPCException;
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::clients::impl::Call;
    using msgpack_rpc::clients::impl::CallList;
    using msgpack_rpc::clients::impl::make_parameters_serializer;
    using msgpack_rpc::messages::MessageID;
    using msgpack_rpc_test::create_parsed_successful_response;
    using msgpack_rpc_test::create_test_logger;
    using msgpack_rpc_test::parse_request;

    const auto logger = create_test_logger();

    const auto executor = msgpack_rpc::executors::create_executor(
        logger, msgpack_rpc::config::ExecutorConfig());
    executor->start();

    const auto timeout = std::chrono::seconds(1);
    const auto list = std::make_shared<CallList>(timeout, executor, logger);

    SECTION("register an RPC") {
        const auto method_name =
            msgpack_rpc::messages::MethodNameView("method1");
        const auto param1 = std::string("abc");

        const auto [request_id, serialized_request, future] =
            list->create(method_name, make_parameters_serializer(param1));

        const auto request = parse_request(serialized_request);
        CHECK(request.method_name() == method_name);
        CHECK(request.id() == request_id);
        CHECK(request.parameters().as<std::string>() ==
            std::forward_as_tuple(param1));

        SECTION("and handle the response") {
            const auto response =
                create_parsed_successful_response(request.id(), "def");

            list->handle(response);

            const auto result = future->get_result();
        }

        SECTION("and try to handle different response") {
            const auto response = create_parsed_successful_response(
                request.id() + static_cast<MessageID>(1), "def");

            list->handle(response);
        }
    }

    SECTION("register an RPC with small timeout") {
        const auto timeout = std::chrono::milliseconds(1);
        const auto list = std::make_shared<CallList>(timeout, executor, logger);

        const auto method_name =
            msgpack_rpc::messages::MethodNameView("method1");
        const auto param1 = std::string("abc");

        const auto [request_id, serialized_request, future] =
            list->create(method_name, make_parameters_serializer(param1));

        const auto request = parse_request(serialized_request);
        CHECK(request.method_name() == method_name);
        CHECK(request.id() == request_id);
        CHECK(request.parameters().as<std::string>() ==
            std::forward_as_tuple(param1));

        SECTION("and handle timeout") {
            try {
                (void)future->get_result_within(std::chrono::seconds(1));
                FAIL();
            } catch (const MsgpackRPCException& e) {
                CHECK(e.status().code() == StatusCode::TIMEOUT);
            }
        }
    }

    SECTION("register several RPC resulting in different request ID") {
        const auto method_name =
            msgpack_rpc::messages::MethodNameView("method1");
        const auto param1 = std::string("abc");

        const auto [request_id1, serialized_request1, future1] =
            list->create(method_name, make_parameters_serializer(param1));
        const auto [request_id2, serialized_request2, future2] =
            list->create(method_name, make_parameters_serializer(param1));

        CHECK(request_id1 != request_id2);
    }
}
