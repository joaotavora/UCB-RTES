add_executable(02-incdecthread-exe main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(02-incdecthread-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(02-incdecthread-exe PRIVATE "-fsanitize=address,undefined")
endif()
