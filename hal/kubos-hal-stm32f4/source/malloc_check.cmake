include(CheckCSourceCompiles)

set(_USED "__attribute__((used))")
check_c_source_compiles("\
#include <reent.h>\n \
#include <stddef.h>\n \
#include <stdlib.h>\n \
${_USED} void * _sbrk(ptrdiff_t size) {return NULL;}\n \
${_USED} void _exit(int code) {}\n \
int main(){\n \
(void) malloc(1); \n\
__malloc_lock(NULL); \n \
__malloc_unlock(NULL); \n \
return 0;}" MALLOC_LOCK_EXISTS)

if (NOT MALLOC_LOCK_EXISTS)
    set (KUBOS_USE_MALLOC_LOCK 1)
endif ()

configure_file("${CMAKE_CURRENT_LIST_DIR}/../kubos-hal-stm32f4/config.h.in"
               kubos-hal-stm32f4/config.h @ONLY)

target_include_directories(kubos-hal-stm32f4 PUBLIC include
                           ${CMAKE_CURRENT_BINARY_DIR})
