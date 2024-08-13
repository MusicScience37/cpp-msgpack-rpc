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
 * \brief Definition of SourceLocationView class.
 */
#include "msgpack_rpc/logging/source_location_view.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("msgpack_rpc::logging::SourceLocationView") {
    using msgpack_rpc::logging::SourceLocationView;

    SECTION("get the current location") {
        const SourceLocationView location =
            MSGPACK_RPC_CURRENT_SOURCE_LOCATION();

        CHECK_THAT(std::string(location.file_path()),
            Catch::Matchers::ContainsSubstring(
                "source_location_view_test.cpp"));
        CHECK(location.line() >= 30U);
        CHECK(location.function() != "");  // NOLINT
    }
}
