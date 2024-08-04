#include "msgpack_rpc/config/toml/parse_toml.h"

#include <chrono>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/executor_config.h"
#include "msgpack_rpc/config/logging_config.h"
#include "msgpack_rpc/config/message_parser_config.h"
#include "msgpack_rpc/config/reconnection_config.h"
#include "msgpack_rpc/config/server_config.h"
#include "msgpack_rpc/logging/log_level.h"

static std::string_view format(msgpack_rpc::logging::LogLevel level) {
    using msgpack_rpc::logging::LogLevel;
    switch (level) {
    case LogLevel::TRACE:
        return "trace";
    case LogLevel::DEBUG:
        return "debug";
    case LogLevel::INFO:
        return "info";
    case LogLevel::WARN:
        return "warn";
    case LogLevel::ERROR:
        return "error";
    case LogLevel::CRITICAL:
        return "critical";
    }
    return "invalid";
}

static std::string format(std::chrono::nanoseconds value) {
    return fmt::format("{:.3f}",
        std::chrono::duration_cast<std::chrono::duration<double>>(value)
            .count());
}

static void format(const msgpack_rpc::config::MessageParserConfig& config) {
    fmt::print(stdout,
        "    message_parser:\n"
        "      read_buffer_size: {}\n",
        config.read_buffer_size());
}

static void format(const msgpack_rpc::config::ExecutorConfig& config) {
    fmt::print(stdout,
        "    executor:\n"
        "      num_transport_threads: {}\n"
        "      num_callback_threads: {}\n",
        config.num_transport_threads(), config.num_callback_threads());
}

static void format(const msgpack_rpc::config::ReconnectionConfig& config) {
    fmt::print(stdout,
        "    reconnection:\n"
        "      initial_waiting_time: {}\n"
        "      max_waiting_time: {}\n"
        "      max_jitter_waiting_time: {}\n",
        format(config.initial_waiting_time()),
        format(config.max_waiting_time()),
        format(config.max_jitter_waiting_time()));
}

int main(int argc, const char** argv) {
    using msgpack_rpc::config::ClientConfig;
    using msgpack_rpc::config::LoggingConfig;
    using msgpack_rpc::config::ServerConfig;

    if (argc != 2) {
        return 2;
    }
    const std::string_view filepath = argv[1];
    fmt::print(stdout, "File: {}\n\n",
        std::filesystem::path(filepath).filename().string());

    try {
        std::unordered_map<std::string, LoggingConfig> logging_configs;
        std::unordered_map<std::string, ClientConfig> client_configs;
        std::unordered_map<std::string, ServerConfig> server_configs;

        msgpack_rpc::config::toml::parse_toml(
            filepath, logging_configs, client_configs, server_configs);

        fmt::print(stdout, "logging:\n");
        for (const auto& [key, config] : logging_configs) {
            fmt::print(stdout,
                "  {}:\n"
                "    filepath: {}\n"
                "    max_file_size: {}\n"
                "    max_files: {}\n"
                "    output_log_level: {}\n",
                key, config.filepath(), config.max_file_size(),
                config.max_files(), format(config.output_log_level()));
        }

        fmt::print(stdout, "client:\n");
        for (const auto& [key, config] : client_configs) {
            fmt::print(stdout,
                "  {}:\n"
                "    uris: [{}]\n"
                "    call_timeout: {}\n",
                key, fmt::join(config.uris(), ", "),
                format(config.call_timeout()));
            format(config.message_parser());
            format(config.executor());
            format(config.reconnection());
        }

        fmt::print(stdout, "server:\n");
        for (const auto& [key, config] : server_configs) {
            fmt::print(stdout,
                "  {}:\n"
                "    uris: [{}]\n",
                key, fmt::join(config.uris(), ", "));
            format(config.message_parser());
            format(config.executor());
        }

        return 0;
    } catch (const std::exception& e) {
        fmt::print(stderr, "{}\n", e.what());
        return 1;
    }
}
