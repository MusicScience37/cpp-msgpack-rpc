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
 * \brief Implementation of clients for benchmarks with random data sizes.
 */
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include <fmt/ostream.h>
#include <lyra/lyra.hpp>

#include "common.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/config.h"

int main(int argc, char** argv) {
    std::size_t num_samples = 0;
    std::string output_file_path;
    const auto cli =
        lyra::cli()
            .add_argument(lyra::opt(num_samples, "Number of samples")
                    .name("--samples")
                    .required()
                    .help("Set the number of samples."))
            .add_argument(lyra::opt(output_file_path, "File path")
                    .name("--output")
                    .required()
                    .help("Set the file path of the output."));
    const auto result = cli.parse({argc, argv});
    if (!result) {
        std::cerr << result.message() << "\n\n" << cli << std::endl;
        return 1;
    }

    const std::vector<msgpack_rpc_test::ServerType> server_types{
        msgpack_rpc_test::ServerType::TCP4, msgpack_rpc_test::ServerType::TCP6
#if MSGPACK_RPC_HAS_UNIX_SOCKETS
        ,
        msgpack_rpc_test::ServerType::UNIX_SOCKET
#endif
    };

    std::vector<std::size_t> data_sizes;
    data_sizes.reserve(num_samples);
    constexpr std::size_t min_size = 16;
#ifdef NDEBUG
    constexpr auto max_size = static_cast<std::size_t>(1024 * 1024);
#else
    constexpr auto max_size = static_cast<std::size_t>(1024);
#endif
    std::mt19937 engine;  // NOLINT: for reproducibility.
    std::uniform_real_distribution<double> log_size_dist(
        std::log(min_size), std::log(max_size));
    std::generate_n(std::back_inserter(data_sizes), num_samples,
        [&engine, &log_size_dist] { return std::exp(log_size_dist(engine)); });

    auto command_client = msgpack_rpc::clients::ClientBuilder()
                              .connect_to(msgpack_rpc_test::COMMAND_SERVER_URI)
                              .build();

    std::ofstream output(output_file_path);
    fmt::print(output, "Protocol,Data Size [byte],Processing Time [sec]\n");

    for (const msgpack_rpc_test::ServerType server_type : server_types) {
        const std::string server_uri = command_client.call<std::string>(
            "prepare", static_cast<int>(server_type));
        auto client = msgpack_rpc::clients::ClientBuilder()
                          .connect_to(server_uri)
                          .build();

        std::string_view server_type_str;
        switch (server_type) {
        case msgpack_rpc_test::ServerType::TCP4:
            server_type_str = "TCPv4";
            break;
        case msgpack_rpc_test::ServerType::TCP6:
            server_type_str = "TCPv6";
            break;
        case msgpack_rpc_test::ServerType::UNIX_SOCKET:
            server_type_str = "Unix";
            break;
        }

        for (const std::size_t data_size : data_sizes) {
            const std::string input(data_size, 'a');
            const auto before_call = std::chrono::steady_clock::now();
            (void)client.call<std::string>("echo", input);
            const auto after_call = std::chrono::steady_clock::now();

            const auto duration = after_call - before_call;
            const double duration_sec =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    duration)
                    .count();
            fmt::print(
                output, "{},{},{}\n", server_type_str, data_size, duration_sec);
        }
    }

    return 0;
}
