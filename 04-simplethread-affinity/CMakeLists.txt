add_executable(simplethread-affinity-exe main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(simplethread-affinity-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(simplethread-affinity-exe PRIVATE "-fsanitize=address,undefined")
endif()

target_link_libraries(simplethread-affinity-exe PRIVATE xpto-lib)
