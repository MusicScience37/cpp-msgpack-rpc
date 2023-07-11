add_executable(integ_executor_interruption_single_thread_executor
               single_thread_executor.cpp)
target_link_libraries(integ_executor_interruption_single_thread_executor
                      PRIVATE ${PROJECT_NAME})

add_executable(integ_executor_interruption_general_executor
               general_executor.cpp)
target_link_libraries(integ_executor_interruption_general_executor
                      PRIVATE ${PROJECT_NAME})
