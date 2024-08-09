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
 * \brief Example of a simple client.
 */
#include <cassert>

#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"

int main() {
    // Create a client.
    msgpack_rpc::clients::Client client =
        msgpack_rpc::clients::ClientBuilder()
            // Specify the URL of the server to connect to.
            .connect_to("tcp://localhost:7136")
            // build() creates a client and starts processing of the client.
            .build();

    // Call a method in the server.
    // Specify the result type in the template parameter.
    const int result = client.call<int>("add", 2, 3);
    assert(result == 5);

    // Stop the client by destructing the client.

    return 0;
}
