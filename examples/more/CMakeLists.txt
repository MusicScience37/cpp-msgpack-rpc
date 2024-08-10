add_executable(more_client more_client.cpp)
target_link_libraries(more_client PRIVATE msgpack_rpc bfg::lyra)

add_executable(more_server more_server.cpp)
target_link_libraries(more_server PRIVATE msgpack_rpc bfg::lyra)
