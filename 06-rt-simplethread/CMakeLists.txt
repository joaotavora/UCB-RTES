add_executable(06-rt-simplethread pthread.cpp)
set_target_properties(06-rt-simplethread PROPERTIES OUTPUT_NAME pthread)

add_custom_command(TARGET 06-rt-simplethread POST_BUILD
    COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:06-rt-simplethread>
    COMMENT "Setting capabilities on target executable"
)
target_link_libraries(06-rt-simplethread PRIVATE xpto-lib)


add_executable(06-rt-simplethread-c pthread.c)
set_target_properties(06-rt-simplethread-c PROPERTIES OUTPUT_NAME pthread-c)

add_custom_command(TARGET 06-rt-simplethread-c POST_BUILD
    COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:06-rt-simplethread-c>
    COMMENT "Setting capabilities on target executable"
)








