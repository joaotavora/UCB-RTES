add_executable(07-hardcoded-sequencer main.cpp)
target_link_libraries(07-hardcoded-sequencer PRIVATE xpto-lib)

add_executable(07-hardcoded-sequencer-c lab1.c)

set_target_properties(07-hardcoded-sequencer PROPERTIES OUTPUT_NAME sequencer)
set_target_properties(07-hardcoded-sequencer-c PROPERTIES OUTPUT_NAME lab1)

xpto_grant_cap_sys_nice(07-hardcoded-sequencer)
xpto_grant_cap_sys_nice(07-hardcoded-sequencer-c)
