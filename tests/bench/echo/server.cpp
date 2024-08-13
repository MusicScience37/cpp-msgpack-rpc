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
 * \brief Implementation of servers for benchmarks.
 */
#include "msgpack_rpc/servers/server.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "common.h"
#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/servers/server_builder.h"

int main() {
    std::optional<msgpack_rpc::servers::Server> echo_server;

    auto command_server =
        msgpack_rpc::servers::ServerBuilder()
            .listen_to(msgpack_rpc_test::COMMAND_SERVER_URI)
            .add_method<std::string(int)>("prepare",
                [&echo_server](int server_type_number) -> std::string {
                    const auto server_type =
                        static_cast<msgpack_rpc_test::ServerType>(
                            server_type_number);
                    echo_server.reset();

                    auto builder = msgpack_rpc::servers::ServerBuilder();
                    switch (server_type) {
                    case msgpack_rpc_test::ServerType::TCP4:
                        builder.listen_to("tcp://127.0.0.1:0");
                        break;
                    case msgpack_rpc_test::ServerType::TCP6:
                        builder.listen_to("tcp://[::1]:0");
                        break;
                    case msgpack_rpc_test::ServerType::UNIX_SOCKET:
                        builder.listen_to("unix://bench_echo.sock");
                        break;
                    default:
                        throw std::runtime_error("Invalid serve type.");
                    }
                    builder.add_method<std::string(std::string)>(
                        "echo", [](const std::string& str) { return str; });
                    echo_server = builder.build();

                    return fmt::format(
                        "{}", echo_server->local_endpoint_uris().front());
                })
            .build();
    command_server.run_until_signal();

    return 0;
}
