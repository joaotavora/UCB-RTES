add_executable(08-clock-times clock_times.c)
add_executable(08-clock-overhead clock-overhead.cpp)
add_executable(08-generic-sequencer generic-sequencer.cpp)
target_link_libraries(08-generic-sequencer PRIVATE xpto-lib)

set_target_properties(08-clock-times PROPERTIES OUTPUT_NAME clock-times)
set_target_properties(08-clock-overhead PROPERTIES OUTPUT_NAME clock-overhead)
set_target_properties(08-generic-sequencer PROPERTIES OUTPUT_NAME generic-sequencer)
