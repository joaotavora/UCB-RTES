cmake_minimum_required(VERSION 3.30)
project(UCB-RTES LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_library(xpto-lib INTERFACE)
target_include_directories(xpto-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(01-simplethread)
add_subdirectory(02-incdecthread)
add_subdirectory(03-twoprocs)
add_subdirectory(04-simplethread-affinity)
add_subdirectory(05-assignment1)

