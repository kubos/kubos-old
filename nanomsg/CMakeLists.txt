cmake_minimum_required(VERSION 3.3)

project(nanomsg)
set(YOTTA_MODULE_NAME nanomsg)

option (NN_STATIC_LIB "" ON)
option (NN_ENABLE_DOC "" OFF)
option (NN_TESTS "" OFF)
option (NN_TOOLS "" OFF)

add_subdirectory("nanomsg")