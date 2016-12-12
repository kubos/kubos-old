# Copyright (C) 2016 Kubos Corporation
if (TARGET_KUBOS_ARM_NONE_EABI_GCC_TOOLCHAIN_INCLUDED)
    return()
endif()
set(TARGET_KUBOS_ARM_NONE_EABI_GCC_TOOLCHAIN_INCLUDED 1)

set(CMAKE_SYSTEM_PROCESSOR "arm926ej-s")
add_definitions("-DTOOLCHAIN_GCC_ARM")
list(APPEND CMAKE_PREFIX_PATH "/opt/buildroot/usr/bin")

macro(gcc_not_found)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        message(" you can install the ARM GCC embedded compiler tools from:")
        message(" https://launchpad.net/gcc-arm-embedded/+download ")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        message(" it is included in the arm-none-eabi-gcc package that you can install")
        message(" with homebrew:\n")
        message("   brew tap ARMmbed/homebrew-formulae")
        message("   brew install arm-none-eabi-gcc")
    endif()
endmacro()

macro(gcc_load_toolchain prefix)
    # find the compiler and associated tools that we need:
    find_program(K_GCC "${prefix}gcc")
    find_program(K_GPP "${prefix}cpp")
    find_program(K_OBJCOPY "${prefix}objcopy")
    if(NOT K_GCC)
        _gcc_not_found("${prefix}gcc")
    endif()
    if(NOT K_GPP)
        _gcc_not_found("${prefix}cpp")
    endif()
    if(NOT K_OBJCOPY)
        _gcc_not_found("${prefix}objcopy")
    endif()

    # set default compilation flags
    IF(CMAKE_BUILD_TYPE MATCHES Debug)
        set(_C_FAMILY_FLAGS_INIT "-fno-exceptions -fno-unwind-tables -ffunction-sections -fdata-sections -Wall -Wextra -gstrict-dwarf")
    ELSE()
        set(_C_FAMILY_FLAGS_INIT "-fno-exceptions -fno-unwind-tables -ffunction-sections -fdata-sections -Wextra -gstrict-dwarf")
    ENDIF()

    set(CMAKE_C_FLAGS_INIT   "${_C_FAMILY_FLAGS_INIT}")
    set(CMAKE_ASM_FLAGS_INIT "-fno-exceptions -fno-unwind-tables -x assembler-with-cpp")
    set(CMAKE_CXX_FLAGS_INIT "--std=gnu++11 ${_C_FAMILY_FLAGS_INIT} -fno-rtti -fno-threadsafe-statics")
    set(CMAKE_MODULE_LINKER_FLAGS_INIT
        "-fno-exceptions -fno-unwind-tables -Wl,--gc-sections -Wl,--sort-common -Wl,--sort-section=alignment"
    )
    if((NOT DEFINED YOTTA_CFG_GCC_PRINTF_FLOAT) OR (YOTTA_CFG_GCC_PRINTF_FLOAT))
        set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wl,-u,_printf_float")
    endif()

    # Set the compiler to ARM-GCC
    if(CMAKE_VERSION VERSION_LESS "3.6.0")
        include(CMakeForceCompiler)
        cmake_force_c_compiler("${K_GCC}" GNU)
        cmake_force_cxx_compiler("${K_GPP}" GNU)
    else()
        # from 3.5 the force_compiler macro is deprecated: CMake can detect
        # arm-none-eabi-gcc as being a GNU compiler automatically
        set(CMAKE_C_COMPILER "${K_GCC}")
        set(CMAKE_CXX_COMPILER "${K_GPP}")
    endif()

endmacro()

gcc_load_toolchain("arm-linux-")

set(_C_FAMILY_FLAGS_INIT   "-std=c99 ${_C_FAMILY_FLAGS_INIT}")