#include "msgpack_rpc/config/toml/parse_toml.h"

#include <cstdio>
#include <exception>
#include <filesystem>
#include <string_view>

#include <fmt/core.h>

#include "msgpack_rpc/config/client_config.h"
#include "msgpack_rpc/config/logging_config.h"
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

        return 0;
    } catch (const std::exception& e) {
        fmt::print(stderr, "{}\n", e.what());
        return 1;
    }
}
