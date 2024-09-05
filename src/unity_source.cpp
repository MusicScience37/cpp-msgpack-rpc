#include "msgpack_rpc/addresses/posix_shared_memory_address.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/addresses/tcp_address.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/addresses/unix_socket_address.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/addresses/uri.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/clients/impl/client_builder_impl.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/clients/server_exception.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/common/msgpack_rpc_exception.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/common/status.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/common/status_code.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/client_config.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/config_parser.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/executor_config.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/logging_config.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/message_parser_config.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/reconnection_config.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/server_config.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/config/toml/parse_toml.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/executors/general_executor.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/executors/single_thread_executor.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/executors/wrapping_executor.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/logging/log_sinks.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/messages/impl/serialization_buffer.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/messages/message_parser.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/messages/message_type.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/messages/serialized_message.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/methods/method_exception.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/methods/method_processor.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/servers/impl/i_server_builder_impl.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/changes_count.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/client_memory_address_calculator.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/posix_shared_memory.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/posix_shm_condition_variable_view.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/posix_shm_mutex_view.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/server_event_queue.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/server_memory_address_calculator.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/shm_stream_reader.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/posix_shm/shm_stream_writer.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/tcp/backends.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/tcp/tcp_backend.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/unix_socket/backends.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/transport/unix_socket/unix_socket_backend.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/util/format_msgpack_object.cpp"  // NOLINT(bugprone-suspicious-include)
#include "msgpack_rpc/util/format_msgpack_object_to_string.cpp"  // NOLINT(bugprone-suspicious-include)
