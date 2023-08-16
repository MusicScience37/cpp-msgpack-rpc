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
 * \brief Test of CallFutureImpl class.
 */
#include "msgpack_rpc/clients/impl/call_future_impl.h"

#include <atomic>
#include <chrono>
#include <memory>
#include <ratio>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <msgpack.hpp>

#include "../../create_test_logger.h"
#include "msgpack_rpc/clients/impl/call_promise.h.h"
#include "msgpack_rpc/clients/impl/i_call_future_impl.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/messages/call_result.h"
#include "msgpack_rpc/messages/message_id.h"

TEST_CASE("msgpack_rpc::clients::impl::CallFutureImpl") {
    using msgpack_rpc::MsgpackRPCException;
    using msgpack_rpc::StatusCode;
    using msgpack_rpc::clients::impl::CallFutureImpl;
    using msgpack_rpc::clients::impl::CallPromise;
    using msgpack_rpc::clients::impl::ICallFutureImpl;
    using msgpack_rpc::messages::CallResult;
    using msgpack_rpc::messages::MessageID;

    const auto logger = msgpack_rpc_test::create_test_logger();

    const auto executor = msgpack_rpc::executors::create_executor(
        logger, msgpack_rpc::config::ExecutorConfig());
    executor->start();

    std::atomic<MessageID> on_timeout_request_id{};
    std::atomic<int> on_timeout_call_count = 0;
    const auto on_timeout = [&on_timeout_call_count, &on_timeout_request_id](
                                msgpack_rpc::messages::MessageID request_id) {
        ++on_timeout_call_count;
        on_timeout_request_id = request_id;
    };

    static constexpr auto request_id = 12345;
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(1);

    const auto promise = std::make_shared<CallPromise>(
        request_id, executor, deadline, on_timeout);
    const std::shared_ptr<ICallFutureImpl> future = promise->future();

    promise->start();

    SECTION("set a result") {
        const auto result_zone = std::make_shared<msgpack::zone>();
        const auto result_object = msgpack::object("abc", *result_zone);
        const auto result =
            CallResult::create_result(result_object, result_zone);

        REQUIRE_NOTHROW(promise->set(result));

        SECTION("get the result") {
            const CallResult received_result = future->get_result();

            CHECK(received_result.result_as<std::string_view>() == "abc");
        }

        SECTION("wait the result") {
            const auto timeout = std::chrono::seconds(1);
            const CallResult received_result =
                future->get_result_within(timeout);

            CHECK(received_result.result_as<std::string_view>() == "abc");
        }
    }

    SECTION("wait the result without a result") {
        const auto timeout = std::chrono::milliseconds(1);
        REQUIRE_THROWS((void)future->get_result_within(timeout));
    }

    SECTION("handle timeout") {
        const auto deadline =
            std::chrono::steady_clock::now() + std::chrono::milliseconds(1);

        const auto promise = std::make_shared<CallPromise>(
            request_id, executor, deadline, on_timeout);
        const std::shared_ptr<ICallFutureImpl> future = promise->future();

        promise->start();

        try {
            (void)future->get_result_within(std::chrono::seconds(1));
            FAIL();
        } catch (const MsgpackRPCException& e) {
            CHECK(e.status().code() == StatusCode::TIMEOUT);
        }
    }
}
