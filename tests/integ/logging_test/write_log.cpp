#include <cstddef>
#include <iostream>
#include <memory>
#include <string>

#include <lyra/lyra.hpp>

#include "msgpack_rpc/config/config_parser.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/logging/i_log_sink.h"
#include "msgpack_rpc/logging/log_sinks.h"
#include "msgpack_rpc/logging/logger.h"

int main(int argc, char** argv) {
    msgpack_rpc::config::LoggingConfig config;
    std::shared_ptr<msgpack_rpc::logging::ILogSink> log_sink;
    bool quiet = false;
    std::string config_file_path;
    std::string config_name;
    const auto cli =
        lyra::cli()
            .add_argument(lyra::opt(config_file_path, "file path")
                    .name("--config-file")
                    .optional()
                    .help("Configuration file."))
            .add_argument(lyra::opt(config_name, "name")
                    .name("--config-name")
                    .optional()
                    .help("Configuration name."))
            .add_argument(
                lyra::opt(quiet).name("--quiet").name("-q").optional().help(
                    "Do not show logs."));
    const auto result = cli.parse({argc, argv});
    if (!result) {
        std::cerr << result.message() << "\n\n";
        std::cerr << cli << std::endl;
        return 1;
    }

    if (quiet) {
        // NOP
    } else {
        if (config_file_path.empty() || config_name.empty()) {
            const auto max_files = static_cast<std::size_t>(1);
            config.max_files(max_files);
        } else {
            msgpack_rpc::config::ConfigParser parser;
            parser.parse(config_file_path);
            config = parser.logging_config(config_name);
        }
        log_sink = msgpack_rpc::logging::create_log_sink_from_config(config);
    }
    const auto logger = msgpack_rpc::logging::Logger::create(
        log_sink, config.output_log_level());

    MSGPACK_RPC_TRACE(logger, "Trace.");
    MSGPACK_RPC_DEBUG(logger, "Debug.");
    MSGPACK_RPC_INFO(logger, "Information.");
    MSGPACK_RPC_WARN(logger, "Warning.");
    MSGPACK_RPC_ERROR(logger, "Error.");
    MSGPACK_RPC_CRITICAL(logger, "Critical.");

    MSGPACK_RPC_INFO(logger, "Log with an argument: {}", 123);

    return 0;
}
