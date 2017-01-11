include(CheckCSourceCompiles)

check_c_source_compiles("\
#include <reent.h>\n \
#include <stddef.h>\n \
void * _sbrk(ptrdiff_t size) {return NULL;}\n \
void _exit(int code) {}\n \
void __malloc_lock(struct _reent *r){}\n \
void __malloc_unlock(struct _reent *r){}\n \
int main(){return 0;}" KUBOS_USE_MALLOC_LOCK)

configure_file("${CMAKE_CURRENT_LIST_DIR}/../kubos-hal-stm32f4/config.h.in"
               kubos-hal-stm32f4/config.h @ONLY)

target_include_directories(kubos-hal-stm32f4 PUBLIC include
                           ${CMAKE_CURRENT_BINARY_DIR})
