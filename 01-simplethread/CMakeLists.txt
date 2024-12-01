add_executable(simplethread-exe main.c)
add_executable(simplethread++-exe main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(simplethread++-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(simplethread++-exe PRIVATE "-fsanitize=address,undefined")
endif()
