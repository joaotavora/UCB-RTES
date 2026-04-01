add_executable(05-assignment-1 pthread.cpp)
target_link_libraries(05-assignment-1 PRIVATE xpto-lib)

set_target_properties(05-assignment-1 PROPERTIES OUTPUT_NAME "pthread")
