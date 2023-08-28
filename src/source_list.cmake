set(SOURCE_FILES
    msgpack_rpc/addresses/tcp_address.cpp
    msgpack_rpc/addresses/uri.cpp
    msgpack_rpc/clients/impl/client_builder_impl.cpp
    msgpack_rpc/clients/impl/server_exception.cpp
    msgpack_rpc/common/msgpack_rpc_exception.cpp
    msgpack_rpc/common/status.cpp
    msgpack_rpc/common/status_code.cpp
    msgpack_rpc/config/client_config.cpp
    msgpack_rpc/config/executor_config.cpp
    msgpack_rpc/config/logging_config.cpp
    msgpack_rpc/config/message_parser_config.cpp
    msgpack_rpc/config/reconnection_config.cpp
    msgpack_rpc/config/server_config.cpp
    msgpack_rpc/executors/general_executor.cpp
    msgpack_rpc/executors/single_thread_executor.cpp
    msgpack_rpc/executors/wrapping_executor.cpp
    msgpack_rpc/logging/log_sinks.cpp
    msgpack_rpc/messages/message_parser.cpp
    msgpack_rpc/messages/message_type.cpp
    msgpack_rpc/methods/method_exception.cpp
    msgpack_rpc/methods/method_processor.cpp
    msgpack_rpc/servers/impl/i_server_builder_impl.cpp
    msgpack_rpc/transport/tcp/backends.cpp
    msgpack_rpc/transport/tcp/tcp_backend.cpp
    msgpack_rpc/util/format_msgpack_object_to_string.cpp
)
