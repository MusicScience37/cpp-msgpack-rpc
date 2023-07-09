#include <cstddef>
#include <memory>
#include <string>

#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/log_sinks.h"
#include "msgpack_rpc/logging/logger.h"

int main() {
    const auto filepath = std::string_view("msgpack_rpc_integ_logging.log");
    const auto max_file_size = static_cast<std::size_t>(1024U * 1024U);
    const auto max_files = static_cast<std::size_t>(1);
    const auto log_sink = msgpack_rpc::logging::create_rotating_file_log_sink(
        filepath, max_file_size, max_files);
    const auto output_log_level = msgpack_rpc::logging::LogLevel::DEBUG;
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
