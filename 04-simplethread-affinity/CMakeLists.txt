add_executable(04-simplethread-affinity main.cpp)
add_executable(04-simplethread-affinity-c main.c)

add_custom_command(TARGET 04-simplethread-affinity POST_BUILD
  COMMENT "Setting capabilities on '$<TARGET_FILE:04-simplethread-affinity>'"
  COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:04-simplethread-affinity>
)

add_custom_command(TARGET 04-simplethread-affinity-c POST_BUILD
  COMMENT "Setting capabilities on '$<TARGET_FILE:04-simplethread-affinity-c>'"
  COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:04-simplethread-affinity-c>
)

target_link_libraries(04-simplethread-affinity PRIVATE xpto-lib)
