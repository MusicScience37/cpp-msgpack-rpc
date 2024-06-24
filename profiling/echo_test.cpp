/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of simple echo server.
 */
#include <gperftools/profiler.h>

#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

int main() {
    const auto logger = msgpack_rpc::logging::Logger::create();

    const auto server_uri = std::string("tcp://localhost:0");

    msgpack_rpc::servers::Server server =
        msgpack_rpc::servers::ServerBuilder(logger)
            .listen_to(server_uri)
            .add_method<std::string(std::string)>(
                "echo", [](const std::string& str) { return str; })
            .build();
    server.start();

    msgpack_rpc::clients::Client client =
        msgpack_rpc::clients::ClientBuilder(logger)
            .connect_to(server.local_endpoint_uris().at(0))
            .build();
    client.start();

    ProfilerStart("profiling_echo.prof");

    constexpr std::size_t num_repetitions = 100000;
    for (std::size_t i = 0; i < num_repetitions; ++i) {
        client.call<std::string>("echo", "abc");
    }

    ProfilerStop();

    client.stop();
    server.stop();

    return 0;
}
