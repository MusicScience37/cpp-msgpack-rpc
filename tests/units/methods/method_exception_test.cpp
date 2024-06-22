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
 * \brief Test of MethodException class.
 */
#include "msgpack_rpc/methods/method_exception.h"

#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("msgpack_rpc::methods::MethodException") {
    using msgpack_rpc::methods::MethodException;

    SECTION("create from a string") {
        const auto exception = MethodException("abc");

        CHECK(exception.object().as<std::string_view>() == "abc");
    }

    SECTION("create from an integer") {
        const auto exception = MethodException(12345);

        CHECK(exception.object().as<int>() == 12345);
    }

    SECTION("get a message") {
        const auto exception = MethodException("abc");

        // This message is not used in implementation for now.
        CHECK(exception.what() != std::string_view());
    }
}
