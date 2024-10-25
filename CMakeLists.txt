cmake_minimum_required(VERSION 3.29)
project(fireDetection C)

set(CMAKE_C_STANDARD 11)

add_executable(fireDetection main.c
        functions.c
        functions.h)
