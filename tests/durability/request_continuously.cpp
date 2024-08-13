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
 * \brief Implementation of a client requesting continuously.
 */
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

#include <fmt/format.h>
#include <lyra/lyra.hpp>

#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/common/msgpack_rpc_exception.h"
#include "msgpack_rpc/common/status_code.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/config_parser.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/logging/logger.h"

int main(int argc, const char** argv) {
    std::uint32_t test_seconds{3U};
    std::string config_file_path;
    std::string server_uri;
    std::string log_file_path = "request_continuously.log";
    const auto cli = lyra::cli()
                         .add_argument(lyra::opt(test_seconds, "seconds")
                                           .name("--seconds")
                                           .name("-s")
                                           .optional()
                                           .help("Number of test seconds."))
                         .add_argument(lyra::opt(config_file_path, "path")
                                           .name("--config")
                                           .required()
                                           .help("Configuration file path."))
                         .add_argument(lyra::opt(server_uri, "URI")
                                           .name("--uri")
                                           .required()
                                           .help("Server URI."))
                         .add_argument(lyra::opt(log_file_path, "path")
                                           .name("--log")
                                           .optional()
                                           .help("Log file path."));
    const auto result = cli.parse({argc, argv});
    if (!result) {
        std::cerr << result.message() << "\n\n";
        std::cerr << cli << std::endl;
        return 1;
    }
    const auto test_duration = std::chrono::seconds(test_seconds);

    msgpack_rpc::config::ConfigParser config_parser;
    config_parser.parse(config_file_path);

    const auto logger =
        msgpack_rpc::logging::Logger::create(msgpack_rpc::config::LoggingConfig(
            config_parser.logging_config("durability_test"))
                                                 .file_path(log_file_path));
    MSGPACK_RPC_INFO(
        logger, "Test duration: {} seconds", test_duration.count());
    MSGPACK_RPC_INFO(logger, "Server URI: {}", server_uri);

    auto client = msgpack_rpc::clients::ClientBuilder(
        config_parser.client_config("durability_test"), logger)
                      .connect_to(server_uri)
                      .build();

    const auto finish_time = std::chrono::steady_clock::now() + test_duration;
    std::uint64_t num_requests = 0;
    while (std::chrono::steady_clock::now() < finish_time) {
        {
            const auto input = std::string("abc");
            const auto output = client.call<std::string>("echo", input);
            if (input != output) {
                throw msgpack_rpc::MsgpackRPCException(
                    msgpack_rpc::StatusCode::UNEXPECTED_ERROR,
                    fmt::format(
                        "Wrong output. Input: {}, Output: {}", input, output));
            }
            ++num_requests;
        }
        {
            const int input1 = 2;
            const int input2 = 3;
            const auto output = client.call<int>("add", input1, input2);
            if (output != input1 + input2) {
                throw msgpack_rpc::MsgpackRPCException(
                    msgpack_rpc::StatusCode::UNEXPECTED_ERROR,
                    fmt::format("Wrong output. Inputs: {}, {}, Output: {}",
                        input1, input2, output));
            }
            ++num_requests;
        }
    }

    MSGPACK_RPC_INFO(logger, "Finished with {} requests.", num_requests);

    return 0;
}
