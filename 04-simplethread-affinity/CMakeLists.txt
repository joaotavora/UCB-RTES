add_executable(simplethread-affinity-exe main.cpp)
add_executable(simplethread-affinity-c-exe main.c)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(simplethread-affinity-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(simplethread-affinity-exe PRIVATE "-fsanitize=address,undefined")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(simplethread-affinity-c-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(simplethread-affinity-c-exe PRIVATE "-fsanitize=address,undefined")
endif()

add_custom_command(TARGET simplethread-affinity-exe POST_BUILD
    COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:simplethread-affinity-exe>
    COMMENT "Setting capabilities on target executable"
)

add_custom_command(TARGET simplethread-affinity-c-exe POST_BUILD
    COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:simplethread-affinity-c-exe>
    COMMENT "Setting capabilities on target executable"
)

target_link_libraries(simplethread-affinity-exe PRIVATE xpto-lib)
