#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/executors/executors.h"
#include "msgpack_rpc/logging/log_level.h"
#include "msgpack_rpc/logging/log_sinks.h"
#include "msgpack_rpc/logging/logger.h"

int main() {
    const auto logger = msgpack_rpc::logging::Logger::create(
        msgpack_rpc::logging::create_stdout_log_sink(),
        msgpack_rpc::logging::LogLevel::TRACE);
    const auto executor_config = msgpack_rpc::config::ExecutorConfig()
                                     .num_transport_threads(2)
                                     .num_callback_threads(4);
    const auto executor =
        msgpack_rpc::executors::create_executor(logger, executor_config);

    executor->run_until_interruption();

    return 0;
}
