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
 * \brief Implementation of a server for durability tests.
 */
#include <iostream>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <lyra/lyra.hpp>

#include "msgpack_rpc/config/config_parser.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

int main(int argc, const char** argv) {
    std::string config_file_path;
    std::vector<std::string> server_uris;
    std::string log_file_path = "serve_methods.log";
    const auto cli = lyra::cli()
                         .add_argument(lyra::opt(config_file_path, "path")
                                           .name("--config")
                                           .required()
                                           .help("Configuration file path."))
                         .add_argument(lyra::opt(server_uris, "URI")
                                           .name("--uri")
                                           .cardinality(1, 10)
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

    msgpack_rpc::config::ConfigParser config_parser;
    config_parser.parse(config_file_path);

    const auto logger =
        msgpack_rpc::logging::Logger::create(msgpack_rpc::config::LoggingConfig(
            config_parser.logging_config("durability_test"))
                                                 .filepath(log_file_path));
    MSGPACK_RPC_INFO(logger, "Server URIs: {}", fmt::join(server_uris, ", "));

    msgpack_rpc::servers::ServerBuilder builder{
        config_parser.server_config("durability_test"), logger};
    for (const std::string& uri : server_uris) {
        builder.listen_to(uri);
    }

    builder.add_method<std::string(std::string)>(
        "echo", [](const std::string& str) { return str; });
    builder.add_method<int(int, int)>(
        "add", [](int x, int y) { return x + y; });

    auto server = builder.build();
    server.run_until_signal();

    MSGPACK_RPC_INFO(logger, "Stopped server.");

    return 0;
}
