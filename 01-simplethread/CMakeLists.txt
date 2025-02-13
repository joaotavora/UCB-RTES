add_executable(01-simplethread-exe main.c)
add_executable(01-simplethread++-exe main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(01-simplethread++-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(01-simplethread++-exe PRIVATE "-fsanitize=address,undefined")
endif()
