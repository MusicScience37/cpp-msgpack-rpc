#include "addresses/tcp_address_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "addresses/unix_socket_address_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "addresses/uri_test.cpp"    // NOLINT(bugprone-suspicious-include)
#include "catch_event_listener.cpp"  // NOLINT(bugprone-suspicious-include)
#include "clients/client_test.cpp"   // NOLINT(bugprone-suspicious-include)
#include "clients/impl/call_future_impl_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "clients/impl/call_list_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "clients/impl/client_impl_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "clients/impl/parameters_serializer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "clients/server_exception_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "common/status_code_test.cpp"    // NOLINT(bugprone-suspicious-include)
#include "common/status_test.cpp"         // NOLINT(bugprone-suspicious-include)
#include "config/client_config_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/config_parser_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/executor_config_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/logging_config_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/message_parser_config_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/reconnection_config_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/server_config_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/toml/parse_toml_client_server_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/toml/parse_toml_common_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/toml/parse_toml_logging_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "config/toml/parse_toml_root_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "create_test_logger.cpp"  // NOLINT(bugprone-suspicious-include)
#include "executors/general_executor_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "executors/single_thread_executor_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "executors/timer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "executors/wrapping_executor_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "logging/source_location_view_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/call_result_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/impl/parse_message_from_object_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/impl/serialization_buffer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/impl/sharable_binary_header_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/message_parser_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/message_serializer_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/message_type_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/method_name_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/method_name_view_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/parsed_parameters_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "messages/serialized_message_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "methods/functional_method_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "methods/method_exception_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "methods/method_processor_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "servers/impl/server_builder_impl_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "servers/impl/server_impl_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "test_main.cpp"  // NOLINT(bugprone-suspicious-include)
#include "transport/async_connect_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "transport/backend_list_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "transport/connection_list_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "transport/connection_wrapper_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "util/format_msgpack_object_test.cpp"  // NOLINT(bugprone-suspicious-include)
#include "util/format_msgpack_object_to_string_test.cpp"  // NOLINT(bugprone-suspicious-include)
