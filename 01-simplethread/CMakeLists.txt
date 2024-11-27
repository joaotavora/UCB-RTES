add_executable(simplethread-exe main.c)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(simplethread-exe PRIVATE "-fsanitize=thread")
  target_link_options(simplethread-exe PRIVATE "-fsanitize=thread")
endif()
target_link_libraries(simplethread-exe PRIVATE pthread)
