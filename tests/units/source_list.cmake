set(SOURCE_FILES
    addresses/tcp_address_test.cpp
    addresses/uri_test.cpp
    catch_event_listener.cpp
    clients/client_test.cpp
    clients/impl/call_future_impl_test.cpp
    clients/impl/call_list_test.cpp
    clients/impl/client_impl_test.cpp
    clients/impl/parameters_serializer_test.cpp
    clients/server_exception_test.cpp
    common/status_code_test.cpp
    common/status_test.cpp
    config/client_config_test.cpp
    config/config_parser_test.cpp
    config/executor_config_test.cpp
    config/logging_config_test.cpp
    config/message_parser_config_test.cpp
    config/reconnection_config_test.cpp
    config/server_config_test.cpp
    config/toml/parse_toml_client_server_test.cpp
    config/toml/parse_toml_common_test.cpp
    config/toml/parse_toml_logging_test.cpp
    config/toml/parse_toml_root_test.cpp
    create_test_logger.cpp
    executors/general_executor_test.cpp
    executors/single_thread_executor_test.cpp
    executors/timer_test.cpp
    executors/wrapping_executor_test.cpp
    logging/source_location_view_test.cpp
    messages/call_result_test.cpp
    messages/impl/parse_message_from_object_test.cpp
    messages/impl/sharable_binary_header_test.cpp
    messages/message_parser_test.cpp
    messages/message_serializer_test.cpp
    messages/message_type_test.cpp
    messages/method_name_test.cpp
    messages/method_name_view_test.cpp
    messages/parsed_parameters_test.cpp
    messages/serialized_message_test.cpp
    methods/functional_method_test.cpp
    methods/method_exception_test.cpp
    methods/method_processor_test.cpp
    servers/impl/server_builder_impl_test.cpp
    servers/impl/server_impl_test.cpp
    test_main.cpp
    transport/async_connect_test.cpp
    transport/backend_list_test.cpp
    transport/connection_list_test.cpp
    transport/connection_wrapper_test.cpp
    util/format_msgpack_object_test.cpp
    util/format_msgpack_object_to_string_test.cpp
)
