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
 * \brief Example of a simple server.
 */
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

int main() {
    // Create a server.
    msgpack_rpc::servers::Server server =
        msgpack_rpc::servers::ServerBuilder()
            // Register a method.
            // Specify the signature of the method in the template parameter.
            .add_method<int(int, int)>(
                "add", [](int x, int y) { return x + y; })
            // Specify the URI of the server.
            .listen_to("tcp://localhost:7136")
            // build() creates a server and starts processing of the server.
            .build();

    // Run the server until this process receives SIGINT or SIGTERM.
    // When this function returns, the server is automatically stopped.
    server.run_until_signal();

    return 0;
}
