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
 * \brief Test of MethodName class.
 */
#include "msgpack_rpc/messages/method_name.h"

#include <catch2/catch_test_macros.hpp>

#include "msgpack_rpc/messages/method_name_view.h"

TEST_CASE("msgpack_rpc::messages::MethodName") {
    using msgpack_rpc::messages::MethodName;
    using msgpack_rpc::messages::MethodNameView;

    SECTION("create from std::string") {
        const std::string str = "abc";

        const MethodName method_name = str;

        CHECK(method_name.name() == str);
    }

    SECTION("create from std::string_view") {
        const std::string_view str = "abc";

        const MethodName method_name = str;

        CHECK(method_name.name() == str);
    }

    SECTION("create from pointer and size") {
        const std::string_view str = "abc";

        const MethodName method_name = MethodName(str.data(), str.size());

        CHECK(method_name.name() == str);
    }

    SECTION("create from raw pointer") {
        const std::string_view str = "abc";

        const MethodName method_name = "abc";

        CHECK(method_name.name() == str);
    }

    SECTION("create from std::string") {
        const MethodNameView str = "abc";

        const auto method_name = static_cast<MethodName>(str);

        CHECK(method_name.name() == str.name());
    }
}
