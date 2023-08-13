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
 * \brief Test of BackendList class.
 */
#include "msgpack_rpc/transport/backend_list.h"

#include <memory>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <trompeloeil.hpp>

#include "mock_backend.h"
#include "msgpack_rpc/transport/i_backend.h"

TEST_CASE("msgpack_rpc::transport::BackendList") {
    using msgpack_rpc::transport::BackendList;
    using msgpack_rpc::transport::IBackend;
    using msgpack_rpc_test::MockBackend;

    BackendList list;

    SECTION("add a backend") {
        const auto scheme1 = std::string_view("scheme1");
        const auto backend1 = std::make_shared<MockBackend>();
        ALLOW_CALL(*backend1, scheme()).RETURN(scheme1);

        REQUIRE_NOTHROW(list.append(backend1));

        SECTION("and add another backend") {
            const auto scheme2 = std::string_view("scheme2");
            const auto backend2 = std::make_shared<MockBackend>();
            ALLOW_CALL(*backend2, scheme()).RETURN(scheme2);

            REQUIRE_NOTHROW(list.append(backend2));

            SECTION("and find a backend") {
                std::shared_ptr<IBackend> result;
                REQUIRE_NOTHROW(result = list.find(scheme1));

                REQUIRE(result->scheme() == scheme1);
            }

            SECTION("and find a backend to fail") {
                CHECK_THROWS((void)list.find("invalid"));
            }
        }

        SECTION("and try to add another backend with the same") {
            const auto& scheme2 = scheme1;
            const auto backend2 = std::make_shared<MockBackend>();
            ALLOW_CALL(*backend2, scheme()).RETURN(scheme2);

            REQUIRE_THROWS(list.append(backend2));
        }
    }
}
