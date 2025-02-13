add_executable(07-hardcoded-sequencer main.cpp)
target_link_libraries(07-hardcoded-sequencer PRIVATE xpto-lib)

add_executable(07-hardcoded-sequencer-c lab1.c)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_options(07-hardcoded-sequencer PRIVATE "-fsanitize=address,undefined")
  target_link_options(07-hardcoded-sequencer PRIVATE "-fsanitize=address,undefined")
endif()

set_target_properties(07-hardcoded-sequencer PROPERTIES OUTPUT_NAME sequencer)
set_target_properties(07-hardcoded-sequencer-c PROPERTIES OUTPUT_NAME lab1)

add_custom_command(TARGET 07-hardcoded-sequencer POST_BUILD
  COMMENT "Setting capabilities on '$<TARGET_FILE:07-hardcoded-sequencer>'"
  # man 7 capabilities tells about CAP_SYS_NICE
  #
  # man 3 cap_from_text tells about what +ep achieves (adds 'effective'
  # and 'permitted' to that capability)
  COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:07-hardcoded-sequencer>
)

add_custom_command(TARGET 07-hardcoded-sequencer-c POST_BUILD
  COMMENT "Setting capabilities on '$<TARGET_FILE:07-hardcoded-sequencer-c>'"
  COMMAND sudo setcap CAP_SYS_NICE+ep $<TARGET_FILE:07-hardcoded-sequencer-c>
)
