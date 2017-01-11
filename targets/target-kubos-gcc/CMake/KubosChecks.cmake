include (CheckCSourceCompiles)
include (CheckFunctionExists)
include (CheckIncludeFiles)

function (kubos_check_include_file HEADER VAR)
    if (TARGET_LIKE_ARM_NONE_EABI_GCC)
        check_c_source_compiles("#include \"${HEADER}\"\n \
        #include <stddef.h>\n \
        #include <unistd.h>\n \
        #include <sys/stat.h>\n \
        void * _sbrk(ptrdiff_t size){return NULL;}\n \
        ssize_t _write(int fd, const void *ptr, size_t len){return 0;}\n \
        int _close(int fd){return 0;}\n \
        int _fstat(int fd, struct stat *st){return 0;}\n \
        int _isatty(int fd){return 0;}\n \
        off_t _lseek(int fd, _off_t ptr, int dir){return 0;}\n \
        ssize_t _read(int fd, void *ptr, size_t len){return 0;}\n \
        void _exit(int code) {}\n \
        int main(void){return 0;}\
        " ${VAR})
    else ()
        check_include_files("${HEADER}" ${VAR})
    endif ()
endfunction ()

function (kubos_check_function_exists FN VAR)
    if (TARGET_LIKE_ARM_NONE_EABI_GCC)
        list (APPEND CMAKE_REQUIRED_LIBRARIES kubos-hal-stm32f4)
    endif ()
    check_function_exists(${FN} ${VAR})
endfunction ()
