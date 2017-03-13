# Copyright (C) 2014-2015 ARM Limited. All rights reserved.

cmake_minimum_required(VERSION 2.8)

# default to C99
# Enable extra warnings for debug builds
set(CMAKE_C_FLAGS_DEBUG "-std=gnu99 -Wall -Wextra -Wshadow  -Wstrict-overflow -fno-strict-aliasing -Wno-unused-parameter" CACHE STRING "")
set(CMAKE_C_FLAGS "-std=gnu99" CACHE STRING "")
set(CMAKE_CXX_FLAGS "-std=gnu++11" CACHE STRING "")

# check that we are actually running on Linux, if we're not then we may pull in
# incorrect dependencies.
if(NOT (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux"))
    message(FATAL_ERROR "This Linux native target will not work on non-Linux platforms (your platform is ${CMAKE_HOST_SYSTEM_NAME}), use `yotta target` to set the target.")
endif()

