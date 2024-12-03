add_executable(twoprocs-exe main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(twoprocs-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(twoprocs-exe PRIVATE "-fsanitize=address,undefined")
endif()

target_link_libraries(twoprocs-exe PRIVATE xpto-lib)
