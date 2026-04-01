add_executable(06-rt-simplethread pthread.cpp)
set_target_properties(06-rt-simplethread PROPERTIES OUTPUT_NAME pthread)

xpto_grant_cap_sys_nice(06-rt-simplethread)
target_link_libraries(06-rt-simplethread PRIVATE xpto-lib)


add_executable(06-rt-simplethread-c pthread.c)
set_target_properties(06-rt-simplethread-c PROPERTIES OUTPUT_NAME pthread-c)

xpto_grant_cap_sys_nice(06-rt-simplethread-c)




