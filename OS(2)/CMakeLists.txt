cmake_minimum_required(VERSION 3.28)
project(OS_2_ C)

set(CMAKE_C_STANDARD 23)

add_executable(OS_2_
        schedule_sjf.c
        schedule_priority.c
        schedule_rr.c
        schedule_priority_rr.c)
