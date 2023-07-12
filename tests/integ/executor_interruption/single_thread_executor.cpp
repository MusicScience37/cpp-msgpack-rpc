#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/log_sinks.h"
#include "msgpack_rpc/logging/logger.h"

int main() {
    const auto logger = msgpack_rpc::logging::Logger::create(
        msgpack_rpc::logging::create_stdout_log_sink(),
        msgpack_rpc::logging::LogLevel::TRACE);
    const auto executor =
        msgpack_rpc::executors::create_single_thread_executor(logger);

    executor->run_until_interruption();

    return 0;
}
