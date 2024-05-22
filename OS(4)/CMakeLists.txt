cmake_minimum_required(VERSION 3.28)
project(OS_4_ C)

set(CMAKE_C_STANDARD 23)

add_executable(OS_4_
        client.c
        list.c
        threadpool.c)
