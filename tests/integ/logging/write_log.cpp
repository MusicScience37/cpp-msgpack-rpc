#include <cstddef>
#include <iostream>
#include <memory>
#include <string>

#include <lyra/lyra.hpp>

#include "msgpack_rpc/logging/i_log_sink.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/log_sinks.h"
#include "msgpack_rpc/logging/logger.h"

int main(int argc, char** argv) {
    std::string filepath;
    std::shared_ptr<msgpack_rpc::logging::ILogSink> log_sink;
    bool verbose = false;
    bool quiet = false;
    const auto cli =
        lyra::cli()
            .add_argument(lyra::opt(filepath, "filepath")
                              .name("--out")
                              .name("-o")
                              .optional()
                              .help("Output to file."))
            .add_argument(lyra::opt(verbose)
                              .name("--verbose")
                              .name("-v")
                              .optional()
                              .help("Show more log."))
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
    } else if (filepath.empty()) {
        log_sink = msgpack_rpc::logging::create_stdout_log_sink();
    } else {
        const auto max_file_size = static_cast<std::size_t>(1024U * 1024U);
        const auto max_files = static_cast<std::size_t>(1);
        log_sink = msgpack_rpc::logging::create_rotating_file_log_sink(
            filepath, max_file_size, max_files);
    }
    auto output_log_level = msgpack_rpc::logging::LogLevel::INFO;
    if (verbose) {
        output_log_level = msgpack_rpc::logging::LogLevel::TRACE;
    }
    const auto logger =
        msgpack_rpc::logging::Logger::create(log_sink, output_log_level);

    MSGPACK_RPC_TRACE(logger, "Trace.");
    MSGPACK_RPC_DEBUG(logger, "Debug.");
    MSGPACK_RPC_INFO(logger, "Information.");
    MSGPACK_RPC_WARN(logger, "Warning.");
    MSGPACK_RPC_ERROR(logger, "Error.");
    MSGPACK_RPC_CRITICAL(logger, "Critical.");

    MSGPACK_RPC_INFO(logger, "Log with an argument: {}", 123);

    return 0;
}
