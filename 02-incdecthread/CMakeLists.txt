add_executable(incdecthread-exe main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(incdecthread-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(incdecthread-exe PRIVATE "-fsanitize=address,undefined")
endif()
