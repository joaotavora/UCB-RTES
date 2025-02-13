add_executable(03-twoprocs-exe main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(03-twoprocs-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(03-twoprocs-exe PRIVATE "-fsanitize=address,undefined")
endif()

target_link_libraries(03-twoprocs-exe PRIVATE xpto-lib)
