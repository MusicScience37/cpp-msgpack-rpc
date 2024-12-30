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
 * \brief Example to use various APIs of servers.
 */
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/ranges.h>
#include <lyra/lyra.hpp>

#include "msgpack_rpc/addresses/uri.h"
#include "msgpack_rpc/config/config_parser.h"
#include "msgpack_rpc/logging/logger.h"
#include "msgpack_rpc/methods/method_exception.h"
#include "msgpack_rpc/servers/server.h"
#include "msgpack_rpc/servers/server_builder.h"

void parse_command_line_arguments(int argc, char** argv,
    std::string& config_file_path, std::string& config_name);

int main(int argc, char** argv) {
    std::string config_file_path;
    std::string config_name;
    parse_command_line_arguments(argc, argv, config_file_path, config_name);

    // Parse configuration.
    msgpack_rpc::config::ConfigParser parser;
    parser.parse(config_file_path);

    // Create a logger from a configuration.
    const std::shared_ptr<msgpack_rpc::logging::Logger> logger =
        msgpack_rpc::logging::Logger::create(
            parser.logging_config(config_name));

    // Create a server.
    msgpack_rpc::servers::Server server =
        // Optionally specify configurations of the server and a logger.
        msgpack_rpc::servers::ServerBuilder(
            parser.server_config(config_name), logger)

            /* ****************************************************************
             * Register methods.
             ******************************************************************/
            // Register a method using a function object.
            // Specify the signature of the method in the template parameter.
            .add_method<int(int, int)>(
                "add", [](int x, int y) { return x + y; })
            // A method without results can be added.
            .add_method<void(std::string)>("print",
                [logger](const std::string& str) {
                    MSGPACK_RPC_INFO(logger, "message: {}", str);
                })
            // Register a method which throws an exception.
            // Exceptions are thrown in clients.
            .add_method<void()>("throw",
                []() -> void {
                    throw std::runtime_error("Example exception.");
                })
            // Register a method which throws an exception with a serializable
            // object. Exceptions are thrown in clients.
            .add_method<void(int)>("throw_int",
                [](int val) -> void {
                    throw msgpack_rpc::methods::MethodException(val);
                })

            /* ****************************************************************
             * Configure URIs of the server.
             ******************************************************************/
            // Specify a URI of the server.
            .listen_to("tcp://localhost:8246")
            // Several URIs can be specified by calling multiple times.
            .listen_to("tcp://localhost:8247")
            // Specify a TCP port.
            .listen_to_tcp("localhost", 8248)  // NOLINT(*-magic-numbers)
            // URIs can be also added in configuration files.

            // build() creates a server and starts processing of the server.
            .build();

    // Get the server URIs.
    const std::vector<msgpack_rpc::addresses::URI> server_uris =
        server.local_endpoint_uris();
    MSGPACK_RPC_INFO(logger, "Server URIs: [{}]", fmt::join(server_uris, ", "));

    // Run the server until this process receives SIGINT or SIGTERM.
    // When this function returns, the server is automatically stopped.
    server.run_until_signal();

    // Server can be stopped using stop function.
    server.stop();

    return 0;
}

// Helper functions.

void parse_command_line_arguments(int argc, char** argv,
    std::string& config_file_path, std::string& config_name) {
    // This function parse command line arguments using lyra library.
    config_file_path = "./examples/more/config.toml";
    config_name = "example";
    bool show_help = false;
    const auto cli = lyra::cli()
                         .add_argument(lyra::opt(config_file_path, "file path")
                                 .name("--config-file")
                                 .name("-f")
                                 .optional()
                                 .help("Configuration file."))
                         .add_argument(lyra::opt(config_name, "name")
                                 .name("--config-name")
                                 .name("-n")
                                 .optional()
                                 .help("Configuration name."))
                         .add_argument(lyra::help(show_help));

    const auto result = cli.parse({argc, argv});
    if (!result) {
        std::cerr << result.message() << "\n\n" << cli << std::endl;
        std::exit(1);  // NOLINT
    }
    if (show_help) {
        std::cout << cli << std::endl;
        std::exit(0);  // NOLINT
    }
}
