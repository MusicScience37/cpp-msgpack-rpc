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
 * \brief Definition of CatchEventListener class.
 */
// clang-format off
// These header must be included in this order.
#include <catch2/catch_section_info.hpp>
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
// clang-format on

#include <catch2/catch_test_case_info.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>

#include "create_test_logger.h"
#include "msgpack_rpc/logging/logger.h"

class CatchEventListener : public Catch::EventListenerBase {
public:
    static constexpr std::size_t LINE_LENGTH = 128;

    template <typename... Args>
    explicit CatchEventListener(Args&&... args)
        : Catch::EventListenerBase(std::forward<Args>(args)...) {}

    void testCaseStarting(const Catch::TestCaseInfo& info) override {
        MSGPACK_RPC_INFO(logger_, std::string(LINE_LENGTH, '='));
        MSGPACK_RPC_INFO(logger_, "Start test case {}.", info.name);
    }

    void sectionStarting(const Catch::SectionInfo& info) override {
        MSGPACK_RPC_INFO(logger_, std::string(LINE_LENGTH, '-'));
        MSGPACK_RPC_INFO(logger_, "Start test section {}.", info.name);
    }

    void sectionEnded(const Catch::SectionStats& stats) override {
        MSGPACK_RPC_INFO(logger_, "Finished test section {} with {} seconds.",
            stats.sectionInfo.name, stats.durationInSeconds);
    }

private:
    //! Logger.
    std::shared_ptr<msgpack_rpc::logging::Logger> logger_{
        msgpack_rpc_test::create_test_logger()};
};
// NOLINTNEXTLINE
CATCH_REGISTER_LISTENER(CatchEventListener);
