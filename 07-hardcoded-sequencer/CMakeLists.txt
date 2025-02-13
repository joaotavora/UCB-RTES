add_executable(hardcoded-sequencer main.cpp)
target_link_libraries(hardcoded-sequencer PRIVATE xpto-lib)

add_executable(hardcoded-sequencer-c lab1.c)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_options(hardcoded-sequencer PRIVATE "-fsanitize=address,undefined")
  target_link_options(hardcoded-sequencer PRIVATE "-fsanitize=address,undefined")
endif()

set_target_properties(hardcoded-sequencer PROPERTIES OUTPUT_NAME sequencer)
set_target_properties(hardcoded-sequencer-c PROPERTIES OUTPUT_NAME lab1)

add_custom_command(TARGET hardcoded-sequencer POST_BUILD
  COMMENT "sudo-setting capabilities on '$<TARGET_FILE:hardcoded-sequencer>'"
  # man 7 capabilities tells about CAP_SYS_NICE
  #
  # man 3 cap_from_text tells about what +ep achieves (adds 'effective'
  # and 'permitted' to that capability)
  COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:hardcoded-sequencer>
)

add_custom_command(TARGET hardcoded-sequencer-c POST_BUILD
  COMMENT "sudo-setting capabilities on '$<TARGET_FILE:hardcoded-sequencer-c>'"
  COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:hardcoded-sequencer-c>
)
