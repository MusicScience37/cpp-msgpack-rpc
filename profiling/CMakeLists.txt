add_executable(profiling_echo echo_test.cpp)
target_link_libraries(profiling_echo PRIVATE ${PROJECT_NAME}
                                             gperftools::libprofiler)
