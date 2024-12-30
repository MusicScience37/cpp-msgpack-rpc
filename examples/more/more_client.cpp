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
 * \brief Example to use various APIs of clients.
 */
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <lyra/lyra.hpp>

#include "msgpack_rpc/clients/call_future.h"
#include "msgpack_rpc/clients/client.h"
#include "msgpack_rpc/clients/client_builder.h"
#include "msgpack_rpc/clients/server_exception.h"
#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/config_parser.h"
#include "msgpack_rpc/logging/logger.h"

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

    // Create a client.
    msgpack_rpc::clients::Client client =
        // Optionally specify configurations of the client and a logger.
        msgpack_rpc::clients::ClientBuilder(
            parser.client_config(config_name), logger)

            /* ****************************************************************
             * Configure URIs of the server.
             ******************************************************************/
            // Specify the URI of the server to connect to.
            .connect_to("tcp://localhost:8246")
            // Several URIs can be specified by calling multiple times.
            .connect_to("tcp://localhost:8247")
            // Specify a TCP port.
            .connect_to_tcp("localhost", 8248)  // NOLINT(*-magic-numbers)

            // build() creates a client and starts processing of the client.
            .build();

    /* ************************************************************************
     * Synchronously call methods.
     **************************************************************************/
    // Synchronously call a method.
    {
        // Specify the result type in the template parameter.
        const int result = client.call<int>("add", 2, 3);
        MSGPACK_RPC_INFO(logger, "Result of add(2, 3): {}", result);
    }
    // Synchronously call a method without a result.
    {
        // Specify void as the result type.
        client.call<void>("print", "Test message.");
    }
    // Synchronously call a method which throws exceptions.
    {
        try {
            client.call<void>("throw");
            MSGPACK_RPC_CRITICAL(logger, "No exception was thrown.");
        } catch (const msgpack_rpc::clients::ServerException& exception) {
            MSGPACK_RPC_INFO(
                logger, "Correctly thrown exception: {}", exception.what());
        }
    }
    // Synchronously call a method which throws exceptions using objects.
    {
        try {
            client.call<void>("throw_int", 123);  // NOLINT(*-magic-numbers)
            MSGPACK_RPC_CRITICAL(logger, "No exception was thrown.");
        } catch (const msgpack_rpc::clients::ServerException& exception) {
            MSGPACK_RPC_INFO(
                logger, "Correctly thrown exception: {}", exception.what());
        }
    }

    /* ************************************************************************
     * Asynchronously call methods.
     **************************************************************************/
    // Asynchronously call a method.
    {
        // Specify the result type in the template parameter.
        msgpack_rpc::clients::CallFuture<int> future =
            client.async_call<int>("add", 2, 3);

        // Wait for a response with a timeout.
        // An exception with status code TIMEOUT will be thrown for the timeout.
        int result = future.get_result_within(std::chrono::seconds(1));

        // Wait for a response without specifying a timeout.
        // The timeout in the configuration is used.
        result = future.get_result();

        MSGPACK_RPC_INFO(logger, "Result of add(2, 3): {}", result);
    }
    // Asynchronously call a method without a result.
    {
        // Specify void as the result type.
        msgpack_rpc::clients::CallFuture<void> future =
            client.async_call<void>("print", "Test message.");

        // get_result, get_result_within functions can be called to check
        // whether the method execution has been completed as in std::future
        // class.
        future.get_result();
    }
    // Asynchronously call a method which throws exceptions.
    {
        try {
            // get_result and get_result_within functions throws the exception
            // in the server.
            client.async_call<void>("throw").get_result();
            MSGPACK_RPC_CRITICAL(logger, "No exception was thrown.");
        } catch (const msgpack_rpc::clients::ServerException& exception) {
            MSGPACK_RPC_INFO(
                logger, "Correctly thrown exception: {}", exception.what());
        }
    }

    /* ************************************************************************
     * Notifications.
     **************************************************************************/
    {
        // Send a notification.
        // ALl notifications are processed asynchronously because notifications
        // have no responses.
        client.notify("print", "Test message.");
        // Nothing is returned from clients even when the method in the server
        // is implemented with return values.
        client.notify("add", 2, 3);
        // No exception is thrown from clients even when the method in the
        // server is implemented to throw an exception.
        client.notify("throw");
    }

    client.call<void>("print", "Client finishes.");

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
