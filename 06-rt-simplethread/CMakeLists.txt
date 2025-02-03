add_executable(rt-simplethread-exe pthread.cpp)
set_target_properties(rt-simplethread-exe PROPERTIES OUTPUT_NAME pthread)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(rt-simplethread-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(rt-simplethread-exe PRIVATE "-fsanitize=address,undefined")
endif()
add_custom_command(TARGET rt-simplethread-exe POST_BUILD
    COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:rt-simplethread-exe>
    COMMENT "Setting capabilities on target executable"
)
target_link_libraries(rt-simplethread-exe PRIVATE xpto-lib)


add_executable(rt-simplethread-c-exe pthread.c)
set_target_properties(rt-simplethread-c-exe PROPERTIES OUTPUT_NAME pthread-c)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(rt-simplethread-c-exe PRIVATE "-fsanitize=address,undefined")
  target_link_options(rt-simplethread-c-exe PRIVATE "-fsanitize=address,undefined")
endif()

add_custom_command(TARGET rt-simplethread-c-exe POST_BUILD
    COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:rt-simplethread-c-exe>
    COMMENT "Setting capabilities on target executable"
)








