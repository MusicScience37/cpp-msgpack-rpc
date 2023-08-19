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
 * \brief Definition of a client for tests.
 */
#include "msgpack_rpc/clients/client.h"

#include <stdexcept>
#include <string>
#include <type_traits>

#include <fmt/core.h>
#include <msgpack.hpp>

#include "common.h"
#include "msgpack_rpc/clients/client_builder.h"

int main() {
    auto client = msgpack_rpc::clients::ClientBuilder()
                      .connect_to(msgpack_rpc_test::SERVER_URL)
                      .build();
    client.start();
    const auto res = client.call<std::string>("echo", "test");
    fmt::print("Result: {}\n", res);
    if (res != "test") {
        return 1;
    }
    return 0;
}
