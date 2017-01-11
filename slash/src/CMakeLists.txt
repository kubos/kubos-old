include (KubosChecks)

option (SLASH_ENABLED "Enable Slash" ON)
option (SLASH_ASF "Enable Atmel Software Framework support" OFF)
option (SLASH_CSP "Enable CubeSat Space Protocol support" ON)

file (GLOB SLASH_C_FILES "*.c")
set (SLASH_LIBS PRIVATE kubos-hal)

if (NOT SLASH_ASF)
    list (REMOVE_ITEM SLASH_C_FILES "${CMAKE_CURRENT_LIST_DIR}/slash_asf.c")
endif ()

if (NOT SLASH_CSP)
    list (REMOVE_ITEM SLASH_C_FILES "${CMAKE_CURRENT_LIST_DIR}/slash_csp.c")
else ()
    list (APPEND SLASH_LIBS PRIVATE csp)
endif ()

add_library (slash STATIC ${SLASH_C_FILES})
target_link_libraries (slash ${SLASH_LIBS})
target_include_directories (slash PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})

kubos_check_include_file (termios.h HAVE_TERMIOS_H)
kubos_check_include_file (unistd.h HAVE_UNISTD_H)
kubos_check_include_file (fcntl.h HAVE_FCNTL_H)
kubos_check_function_exists (getopt HAVE_GETOPT)

configure_file (libslash.h.in libslash.h @ONLY)
