add_executable(04-simplethread-affinity main.cpp)
add_executable(04-simplethread-affinity-c main.c)

xpto_grant_cap_sys_nice(04-simplethread-affinity)
xpto_grant_cap_sys_nice(04-simplethread-affinity-c)

target_link_libraries(04-simplethread-affinity PRIVATE xpto-lib)
