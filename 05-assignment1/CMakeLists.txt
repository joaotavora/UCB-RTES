add_executable(05-assignment-1-exe pthread.cpp)
target_link_libraries(05-assignment-1-exe PRIVATE xpto-lib)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(05-assignment-1-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(05-assignment-1-exe PRIVATE "-fsanitize=address,undefined")
endif()

set_target_properties(05-assignment-1-exe PROPERTIES OUTPUT_NAME "pthread")
