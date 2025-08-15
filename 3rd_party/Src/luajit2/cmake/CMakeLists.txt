cmake_minimum_required(VERSION 3.10)

project(luajit C)
set(can_use_assembler TRUE)
enable_language(ASM)

if(NOT LUAJIT_DIR)
  message(FATAL_ERROR "Must set LUAJIT_DIR to build luajit with CMake")
endif()

file(TO_CMAKE_PATH "${LUAJIT_DIR}" LUAJIT_DIR)
set(LJ_DIR ${LUAJIT_DIR}/src)

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/modules"
)

include(GNUInstallDirs)

set(LJ_CROSSCOMPILING OFF)
if (NOT ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "${CMAKE_SYSTEM_NAME}"))
  set(LJ_CROSSCOMPILING ON)
endif()

set(LUAJIT_DISABLE_GC64 OFF CACHE BOOL "Disable GC64 mode for x64")
set(LUA_MULTILIB "lib" CACHE PATH "The name of lib directory." )
set(CMAKE_CROSSCOMPILING ${LJ_CROSSCOMPILING} CACHE BOOL "Cross-compiling")
set(LUAJIT_DISABLE_FFI OFF CACHE BOOL "Permanently disable the FFI extension")
set(LUAJIT_DISABLE_JIT OFF CACHE BOOL "Disable the JIT compiler")
set(LUAJIT_NO_UNWIND OFF CACHE BOOL "Disable the UNWIND")
set(LUAJIT_BUILD_TOOL ON CACHE BOOL "Build the executable of LuaJIT")
set(LUAJIT_DISABLE_SSE2 OFF CACHE BOOL "Disable SSE2 compiler option on x86 processors")
set(LUAJIT_NUMMODE 0 CACHE STRING
"Specify the number mode to use. Possible values:
  0 - Default mode
  1 - Single number mode
  2 - Dual number mode
")
set(LUAJIT_USE_GDBJIT OFF CACHE BOOL "Enable the GDB JIT API")
set(LUAJIT_ENABLE_LUA52COMPAT OFF CACHE BOOL "Enable Lua 5.2 extensions")
set(LUAJIT_ENABLE_AMALG ON CACHE BOOL "Enable amalgamated (single source) build")

if (NINTENDO_DS OR NINTENDO_3DS OR NINTENDO_WII OR NINTENDO_WII_U OR NINTENDO_SWITCH OR VITA)
  unset(LUAJIT_DISABLE_FFI)
  unset(LUAJIT_DISABLE_JIT)
  set(LUAJIT_DISABLE_FFI ON CACHE BOOL "" FORCE)
  set(LUAJIT_DISABLE_JIT ON CACHE BOOL "" FORCE)
  set(LUAJIT_BUILD_TOOL OFF CACHE BOOL "" FORCE)
endif()

include(CheckTypeSize)
include(TestBigEndian)
test_big_endian(LJ_BIG_ENDIAN)

include(CheckCCompilerFlag)

if (MSVC)
  check_c_compiler_flag("-Xclang -fno-stack-protector" LJ_NO_STACK_PROTECTOR)
  if (LJ_NO_STACK_PROTECTOR)
    set(LJ_NO_STACK_PROTECTOR -Xclang -fno-stack-protector)
  endif()

  check_c_compiler_flag("-Xclang -fno-strict-float-cast-overflow" LJ_NO_STRICT_FLOAT_CAST_OVERFLOW)
  if (LJ_NO_STRICT_FLOAT_CAST_OVERFLOW)
    set(LJ_NO_STRICT_FLOAT_CAST_OVERFLOW -Xclang -fno-strict-float-cast-overflow)
  endif()
else()
  check_c_compiler_flag("-fno-stack-protector" LJ_NO_STACK_PROTECTOR)
  if (LJ_NO_STACK_PROTECTOR)
    set(LJ_NO_STACK_PROTECTOR -fno-stack-protector)
  endif()

  check_c_compiler_flag("-fno-strict-float-cast-overflow" LJ_NO_STRICT_FLOAT_CAST_OVERFLOW)
  if (LJ_NO_STRICT_FLOAT_CAST_OVERFLOW)
    set(LJ_NO_STRICT_FLOAT_CAST_OVERFLOW -fno-strict-float-cast-overflow)
  endif()
endif()

include(DetectArchitecture)
detect_architecture(LJ_DETECTED_ARCH)

include(DetectFPUApi)
detect_fpu_mode(LJ_DETECTED_FPU_MODE)
detect_fpu_abi(LJ_DETECTED_FPU_ABI)

include(CheckIncludeFile)
if(NOT WIN32)
    check_include_file("sys/mman.h" LJ_DETECTED_SYS_MMAN_H)
endif()

find_library(LIBM_LIBRARIES NAMES m)
find_library(LIBDL_LIBRARIES NAMES dl)

set(TARGET_ARCH "")
set(DASM_FLAGS -LN)

set(LJ_TARGET_ARCH "")
if("${LJ_DETECTED_ARCH}" STREQUAL "x86")
  set(LJ_TARGET_ARCH "x86")
elseif ("${LJ_DETECTED_ARCH}" STREQUAL "x86_64")
  set(LJ_TARGET_ARCH "x64")
elseif ("${LJ_DETECTED_ARCH}" STREQUAL "AArch64")
  set(LJ_TARGET_ARCH "arm64")
  if (LJ_BIG_ENDIAN)
    set(TARGET_ARCH -D__AARCH64EB__=1)
  endif()
elseif ("${LJ_DETECTED_ARCH}" STREQUAL "ARM")
  set(LJ_TARGET_ARCH "arm")
elseif ("${LJ_DETECTED_ARCH}" STREQUAL "Mips64")
  set(LJ_TARGET_ARCH "mips64")
  if (NOT LJ_BIG_ENDIAN)
    set(TARGET_ARCH -D__MIPSEL__=1)
  endif()
elseif ("${LJ_DETECTED_ARCH}" STREQUAL "Mips")
  set(LJ_TARGET_ARCH "mips")
  if (NOT LJ_BIG_ENDIAN)
    set(TARGET_ARCH -D__MIPSEL__=1)
  endif()
elseif ("${LJ_DETECTED_ARCH}" STREQUAL "PowerPC")
  if (LJ_64)
    set(LJ_TARGET_ARCH "ppc64")
  else()
    set(LJ_TARGET_ARCH "ppc")
  endif()
  if (LJ_BIG_ENDIAN)
    set(TARGET_ARCH -DLJ_ARCH_ENDIAN=LUAJIT_BE)
  else()
    set(TARGET_ARCH -DLJ_ARCH_ENDIAN=LUAJIT_LE)
  endif()
else()
  message(FATAL_ERROR "Unsupported target architecture: '${LJ_DETECTED_ARCH}'")
endif()

if("${LJ_DETECTED_FPU_MODE}" STREQUAL "Hard")
  set(LJ_HAS_FPU 1)
  set(DASM_FLAGS ${DASM_FLAGS} -D FPU)
  set(TARGET_ARCH ${TARGET_ARCH} -DLJ_ARCH_HASFPU=1)
else()
  set(LJ_HAS_FPU 0)
  set(TARGET_ARCH ${TARGET_ARCH} -DLJ_ARCH_HASFPU=0)
endif()

if("${LJ_DETECTED_FPU_ABI}" STREQUAL "Hard")
  set(LJ_ABI_SOFTFP 0)
  set(DASM_FLAGS ${DASM_FLAGS} -D HFABI)
  set(TARGET_ARCH ${TARGET_ARCH} -DLJ_ABI_SOFTFP=0)
else()
  set(LJ_ABI_SOFTFP 1)
  set(TARGET_ARCH ${TARGET_ARCH} -DLJ_ABI_SOFTFP=1)
endif()

set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_TARGET=LUAJIT_ARCH_${LJ_TARGET_ARCH})

if (WIN32 OR MINGW)
  set(DASM_FLAGS ${DASM_FLAGS} -D WIN)
elseif (IOS)
  set(DASM_FLAGS ${DASM_FLAGS} -D IOS)
  set(DASM_FLAGS ${DASM_FLAGS} -D NO_UNWIND)
  set(TARGET_ARCH ${TARGET_ARCH} -DLJ_NO_UNWIND=1)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_NO_UNWIND)
endif()

set(LJ_DEFINITIONS "")

set(LJ_ENABLE_LARGEFILE 1)
if (ANDROID AND (${CMAKE_SYSTEM_VERSION} LESS 21))
  set(LJ_ENABLE_LARGEFILE 0)
elseif (WIN32 OR MINGW)
  set(LJ_ENABLE_LARGEFILE 0)
endif()

if (LJ_ENABLE_LARGEFILE)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS}
      -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE)
endif()

set(LJ_FFI 1)
if (LUAJIT_DISABLE_FFI)
  set(LJ_FFI 0)
endif()

set(LJ_JIT 1)
if (IOS)
  set(LJ_JIT 0)
elseif (LUAJIT_DISABLE_JIT)
  set(LJ_JIT 0)
endif()

if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  set(LJ_64 1)
endif()

set(LJ_GC64 ${LJ_64})

if (LJ_64 AND LUAJIT_DISABLE_GC64 AND ("${LJ_TARGET_ARCH}" STREQUAL "x64"))
  set(LJ_GC64 0)
endif()

set(LJ_FR2 ${LJ_GC64})

set(LJ_NUMMODE_SINGLE 0) # Single-number mode only.
set(LJ_NUMMODE_SINGLE_DUAL 1) # Default to single-number mode.
set(LJ_NUMMODE_DUAL 2) # Dual-number mode only.
set(LJ_NUMMODE_DUAL_SINGLE 3) # Default to dual-number mode.

set(LJ_ARCH_NUMMODE ${LJ_NUMMODE_DUAL})
if (LJ_HAS_FPU)
  set(LJ_ARCH_NUMMODE ${LJ_NUMMODE_DUAL_SINGLE})
endif()

if (("${LJ_TARGET_ARCH}" STREQUAL "x86") OR
    ("${LJ_TARGET_ARCH}" STREQUAL "x64"))
  set(LJ_ARCH_NUMMODE ${LJ_NUMMODE_SINGLE_DUAL})
endif()

if (("${LJ_TARGET_ARCH}" STREQUAL "arm") OR
    ("${LJ_TARGET_ARCH}" STREQUAL "arm64") OR
    ("${LJ_TARGET_ARCH}" STREQUAL "mips") OR
    ("${LJ_TARGET_ARCH}" STREQUAL "mips64"))
  set(LJ_ARCH_NUMMODE ${LJ_NUMMODE_DUAL})
endif()

# Enable or disable the dual-number mode for the VM.
if (((LJ_ARCH_NUMMODE EQUAL LJ_NUMMODE_SINGLE) AND (LUAJIT_NUMMODE EQUAL 2)) OR
    ((LJ_ARCH_NUMMODE EQUAL LJ_NUMMODE_DUAL) AND (LUAJIT_NUMMODE EQUAL 1)))
  message (FATAL_ERROR "No support for this number mode on this architecture")
endif()
if ((LJ_ARCH_NUMMODE EQUAL LJ_NUMMODE_DUAL) OR
    ((LJ_ARCH_NUMMODE EQUAL LJ_NUMMODE_DUAL_SINGLE) AND NOT (LUAJIT_NUMMODE EQUAL 1)) OR
    ((LJ_ARCH_NUMMODE EQUAL LJ_NUMMODE_SINGLE_DUAL) AND (LUAJIT_NUMMODE EQUAL 2)))
  set(LJ_DUALNUM 1)
else()
  set(LJ_DUALNUM 0)
endif()

set(BUILDVM_ARCH_H ${CMAKE_CURRENT_BINARY_DIR}/buildvm_arch.h)
set(DASM_PATH ${LUAJIT_DIR}/dynasm/dynasm.lua)

if (NOT LJ_BIG_ENDIAN)
  set(DASM_FLAGS ${DASM_FLAGS} -D ENDIAN_LE)
else()
  set(DASM_FLAGS ${DASM_FLAGS} -D ENDIAN_BE)
endif()

if (LJ_64)
  set(DASM_FLAGS ${DASM_FLAGS} -D P64)
endif()

if (LJ_FFI)
  set(DASM_FLAGS ${DASM_FLAGS} -D FFI)
endif()

if (LJ_JIT)
  set(DASM_FLAGS ${DASM_FLAGS} -D JIT)
endif()

if (LJ_DUALNUM)
  set(DASM_FLAGS ${DASM_FLAGS} -D DUALNUM)
endif()

set(DASM_ARCH ${LJ_TARGET_ARCH})

if ("${LJ_TARGET_ARCH}" STREQUAL "x64")
  if (NOT LJ_FR2)
    set(DASM_ARCH "x86")
  endif()
endif()

set(TARGET_OS_FLAGS "")
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_LINUX)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_WINDOWS)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_OSX -DTARGET_OS_IPHONE=1)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_OSX -DTARGET_OS_IPHONE=0)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_LINUX)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Haiku")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_POSIX)
elseif ("${CMAKE_SYSTEM_NAME}" MATCHES "(Open|Free|Net)BSD")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_BSD)
elseif (VITA)
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_OTHER -DLJ_TARGET_PSVITA=1)
elseif ("${CMAKE_SYSTEM_NAME}" MATCHES "Nintendo(DS|3DS|Wii|WiiU|Switch)")
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLJ_TARGET_NX -DLUAJIT_OS=LUAJIT_OS_OTHER -DLUAJIT_SECURITY_PRNG=0)
else()
  set(TARGET_OS_FLAGS ${TARGET_OS_FLAGS} -DLUAJIT_OS=LUAJIT_OS_OTHER)
endif()

if (LUAJIT_DISABLE_GC64)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_DISABLE_GC64)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_DISABLE_GC64)
endif()

set(TARGET_ARCH ${TARGET_ARCH} ${TARGET_OS_FLAGS})
set(LJ_DEFINITIONS ${LJ_DEFINITIONS} ${TARGET_OS_FLAGS})

if (LUAJIT_DISABLE_FFI)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_DISABLE_FFI)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_DISABLE_FFI)
endif()
if (LUAJIT_DISABLE_JIT)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_DISABLE_JIT)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_DISABLE_JIT)
endif()
if (LUAJIT_USE_GDBJIT)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_USE_GDBJIT)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_USE_GDBJIT)
endif()
if (LUAJIT_ENABLE_LUA52COMPAT)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_ENABLE_LUA52COMPAT)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_ENABLE_LUA52COMPAT)
endif()

if (NOT LUAJIT_NO_UNWIND AND NOT (WIN32 OR MINGW OR IOS))
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_UNWIND_EXTERNAL)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_UNWIND_EXTERNAL)
endif()

if (("${LUAJIT_NUMMODE}" STREQUAL "1") OR
    ("${LUAJIT_NUMMODE}" STREQUAL "2"))
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_NUMMODE=${LUAJIT_NUMMODE})
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_NUMMODE=${LUAJIT_NUMMODE})
endif()

if (LUAJIT_ENABLE_GDBJIT)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_ENABLE_GDBJIT)
  set(TARGET_ARCH ${TARGET_ARCH} -DLUAJIT_ENABLE_GDBJIT)
endif()

if (NOT WIN32 AND NOT LJ_DETECTED_SYS_MMAN_H)
  set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUAJIT_USE_SYSMALLOC)
endif()

set(VM_DASC_PATH ${LJ_DIR}/vm_${DASM_ARCH}.dasc)

# Build the minilua for host platform
set(MINILUA_EXE minilua)
if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
  set(MINILUA_EXE minilua.exe)
endif()

if (NOT CMAKE_CROSSCOMPILING)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/host/minilua)
  set(MINILUA_PATH $<TARGET_FILE:minilua>)
else()
  make_directory(${CMAKE_CURRENT_BINARY_DIR}/minilua)
  set(MINILUA_PATH ${CMAKE_CURRENT_BINARY_DIR}/minilua/${MINILUA_EXE})

  add_custom_command(OUTPUT ${MINILUA_PATH}
    COMMAND ${CMAKE_COMMAND} -DIOS=${IOS} ${CMAKE_CURRENT_LIST_DIR}/host/minilua
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/minilua
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/minilua)

  add_custom_target(minilua ALL
    DEPENDS ${MINILUA_PATH}
  )
endif()

# Generate luajit.h
set(LJ_LUAJIT_PATH "${CMAKE_CURRENT_BINARY_DIR}/luajit.h")

find_package(Git)
if (GIT_EXECUTABLE AND EXISTS "${LUAJIT_DIR}/.git")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} show -s "--format=%ct"
    OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/luajit_relver.txt"
    WORKING_DIRECTORY ${LUAJIT_DIR}
  )
else()
  configure_file("${LUAJIT_DIR}/.relver" "${CMAKE_CURRENT_BINARY_DIR}/luajit_relver.txt" COPYONLY)
endif()

add_custom_command(OUTPUT ${LJ_LUAJIT_PATH}
  COMMAND ${MINILUA_PATH}
          "${LJ_DIR}/host/genversion.lua"
          "${LJ_DIR}/luajit_rolling.h"
          "${CMAKE_CURRENT_BINARY_DIR}/luajit_relver.txt"
          ${LJ_LUAJIT_PATH}
  DEPENDS minilua)
add_custom_target(lj_luajit_h ALL
  DEPENDS ${LJ_LUAJIT_PATH}
)

# Generate buildvm_arch.h
add_custom_command(OUTPUT ${BUILDVM_ARCH_H}
  COMMAND ${MINILUA_PATH} ${DASM_PATH} ${DASM_FLAGS}
          -o ${BUILDVM_ARCH_H} ${VM_DASC_PATH}
  DEPENDS minilua lj_luajit_h)
add_custom_target(buildvm_arch_h ALL
  DEPENDS ${BUILDVM_ARCH_H}
)

# Build the buildvm for host platform
set(BUILDVM_COMPILER_FLAGS "${TARGET_ARCH}")

set(BUILDVM_COMPILER_FLAGS_PATH
  "${CMAKE_CURRENT_BINARY_DIR}/buildvm_flags.config")
file(WRITE ${BUILDVM_COMPILER_FLAGS_PATH} "${BUILDVM_COMPILER_FLAGS}")

set(BUILDVM_EXE buildvm)
if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
  set(BUILDVM_EXE buildvm.exe)
endif()

if (NOT CMAKE_CROSSCOMPILING)
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/host/buildvm)
  set(BUILDVM_PATH $<TARGET_FILE:buildvm>)
  add_dependencies(buildvm buildvm_arch_h)
else()
  set(BUILDVM_PATH ${CMAKE_CURRENT_BINARY_DIR}/buildvm/${BUILDVM_EXE})

  find_program(DEFAULT_HOST_C_COMPILER "gcc")
  find_program(DEFAULT_HOST_CXX_COMPILER "g++")
  find_program(DEFAULT_HOST_CMAKE_COMMAND "cmake")
  mark_as_advanced(DEFAULT_HOST_C_COMPILER DEFAULT_HOST_CXX_COMPILER DEFAULT_HOST_CMAKE_COMMAND)

  set(HOST_CMAKE_COMMAND ${DEFAULT_HOST_CMAKE_COMMAND} CACHE STRING "Bypass the host CMake command")
  set(HOST_C_COMPILER ${DEFAULT_HOST_C_COMPILER} CACHE STRING "Bypass the host C compiler command")
  set(HOST_CXX_COMPILER ${DEFAULT_HOST_CXX_COMPILER} CACHE STRING "Bypass the host C++ compiler command")

  make_directory(${CMAKE_CURRENT_BINARY_DIR}/buildvm)

  add_custom_command(OUTPUT ${BUILDVM_PATH}
    COMMAND ${HOST_CMAKE_COMMAND} -E env CC="${HOST_C_COMPILER}" CXX="${HOST_CXX_COMPILER}"
            ${HOST_CMAKE_COMMAND} -DIOS=${IOS} ${CMAKE_CURRENT_LIST_DIR}/host/buildvm
                                  -DEXTRA_COMPILER_FLAGS_FILE=${BUILDVM_COMPILER_FLAGS_PATH}
    COMMAND ${HOST_CMAKE_COMMAND} -E env CC="${HOST_C_COMPILER}" CXX="${HOST_CXX_COMPILER}"
            ${HOST_CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/buildvm
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/host/buildvm/CMakeLists.txt
    DEPENDS buildvm_arch_h lj_luajit_h
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/buildvm)

  add_custom_target(buildvm ALL
    DEPENDS ${BUILDVM_PATH}
  )
endif()

set(LJVM_MODE elfasm)
if (APPLE)
  set(LJVM_MODE machasm)
elseif (WIN32 OR MINGW)
  set(LJVM_MODE peobj)
endif()

set(LJ_VM_NAME lj_vm.S)
if ("${LJVM_MODE}" STREQUAL "peobj")
  set(LJ_VM_NAME lj_vm.obj)
endif()

set(LJ_VM_S_PATH ${CMAKE_CURRENT_BINARY_DIR}/${LJ_VM_NAME})
add_custom_command(OUTPUT ${LJ_VM_S_PATH}
  COMMAND ${BUILDVM_PATH} -m ${LJVM_MODE} -o ${LJ_VM_S_PATH}
  DEPENDS buildvm
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)

add_custom_target(lj_gen_vm_s ALL
  DEPENDS ${LJ_VM_S_PATH}
)

if (APPLE AND CMAKE_OSX_DEPLOYMENT_TARGET)
  set_source_files_properties(${LJ_VM_NAME} PROPERTIES COMPILE_FLAGS -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET})
endif()

make_directory(${CMAKE_CURRENT_BINARY_DIR}/jit)
set(LJ_LIBDEF_PATH ${CMAKE_CURRENT_BINARY_DIR}/lj_libdef.h)
set(LJ_RECDEF_PATH ${CMAKE_CURRENT_BINARY_DIR}/lj_recdef.h)
set(LJ_FFDEF_PATH ${CMAKE_CURRENT_BINARY_DIR}/lj_ffdef.h)
set(LJ_BCDEF_PATH ${CMAKE_CURRENT_BINARY_DIR}/lj_bcdef.h)
set(LJ_VMDEF_PATH ${CMAKE_CURRENT_BINARY_DIR}/jit/vmdef.lua)

set(LJ_LIB_SOURCES
  ${LJ_DIR}/lib_base.c ${LJ_DIR}/lib_math.c ${LJ_DIR}/lib_bit.c
  ${LJ_DIR}/lib_string.c ${LJ_DIR}/lib_table.c ${LJ_DIR}/lib_io.c
  ${LJ_DIR}/lib_os.c ${LJ_DIR}/lib_package.c ${LJ_DIR}/lib_debug.c
  ${LJ_DIR}/lib_jit.c ${LJ_DIR}/lib_ffi.c ${LJ_DIR}/lib_buffer.c)
add_custom_command(
  OUTPUT ${LJ_LIBDEF_PATH} ${LJ_VMDEF_PATH} ${LJ_RECDEF_PATH} ${LJ_FFDEF_PATH}
  OUTPUT ${LJ_BCDEF_PATH}
  COMMAND ${BUILDVM_PATH} -m libdef -o ${LJ_LIBDEF_PATH} ${LJ_LIB_SOURCES}
  COMMAND ${BUILDVM_PATH} -m recdef -o ${LJ_RECDEF_PATH} ${LJ_LIB_SOURCES}
  COMMAND ${BUILDVM_PATH} -m ffdef -o ${LJ_FFDEF_PATH} ${LJ_LIB_SOURCES}
  COMMAND ${BUILDVM_PATH} -m bcdef -o ${LJ_BCDEF_PATH} ${LJ_LIB_SOURCES}
  COMMAND ${BUILDVM_PATH} -m vmdef -o ${LJ_VMDEF_PATH} ${LJ_LIB_SOURCES}
  DEPENDS buildvm ${LJ_LIB_SOURCE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)

add_custom_target(lj_gen_headers ALL
  DEPENDS ${LJ_LIBDEF_PATH} ${LJ_RECDEF_PATH} ${LJ_VMDEF_PATH}
  DEPENDS ${LJ_FFDEF_PATH} ${LJ_BCDEF_PATH} ${LJ_LUAJIT_PATH}
)

set(LJ_FOLDDEF_PATH ${CMAKE_CURRENT_BINARY_DIR}/lj_folddef.h)

set(LJ_FOLDDEF_SOURCE ${LJ_DIR}/lj_opt_fold.c)
add_custom_command(
  OUTPUT ${LJ_FOLDDEF_PATH}
  COMMAND ${BUILDVM_PATH} -m folddef -o ${LJ_FOLDDEF_PATH} ${LJ_FOLDDEF_SOURCE}
  DEPENDS ${BUILDVM_PATH}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)

add_custom_target(lj_gen_folddef ALL
  DEPENDS ${LJ_FOLDDEF_PATH}
)
if (APPLE AND CMAKE_OSX_DEPLOYMENT_TARGET)
  add_dependencies(lj_gen_folddef buildvm)
endif()

file(GLOB_RECURSE SRC_LJCORE "${LJ_DIR}/lj_*.c")
file(GLOB_RECURSE SRC_LIBCORE "${LJ_DIR}/lib_*.c")

if (LUAJIT_ENABLE_AMALG)
  set(luajit_sources ${LJ_DIR}/ljamalg.c ${LJ_VM_NAME})
else()
  set(luajit_sources ${SRC_LIBCORE} ${SRC_LJCORE} ${LJ_VM_NAME})
endif()

# Build the luajit static library
add_library(libluajit ${luajit_sources})
set_target_properties(libluajit PROPERTIES OUTPUT_NAME luajit)
add_dependencies(libluajit
  buildvm_arch_h
  buildvm
  lj_gen_headers
  lj_gen_folddef)
if (APPLE AND CMAKE_OSX_DEPLOYMENT_TARGET)
  add_dependencies(libluajit lj_gen_vm_s)
endif()

target_include_directories(libluajit PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(libluajit PUBLIC ${LJ_DIR})

if (BUILD_SHARED_LIBS)
  if(WIN32 OR MINGW)
    set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUA_BUILD_AS_DLL)
  endif()
  if (APPLE AND NOT LUAJIT_ENABLE_GC64)
    target_link_libraries(libluajit "-image_base 7fff04c4a000")
  endif()
endif()

if(LIBM_LIBRARIES)
  target_link_libraries(libluajit ${LIBM_LIBRARIES})
endif()

if (LIBDL_LIBRARIES)
  target_link_libraries(libluajit ${LIBDL_LIBRARIES})
endif()

if (VITA)
  target_compile_definitions(libluajit PRIVATE -DsceRandomGetRandomNumber=sceKernelGetRandomNumber)
  target_link_libraries(libluajit SceLibKernel_stub)
endif()

set(LJ_DEFINITIONS ${LJ_DEFINITIONS} -DLUA_MULTILIB="${LUA_MULTILIB}")
target_compile_definitions(libluajit PRIVATE ${LJ_DEFINITIONS})

if ("${LJ_TARGET_ARCH}" STREQUAL "x86")
  if (CMAKE_COMPILER_IS_CLANGXX OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(libluajit PRIVATE -march=i686 -msse)
    if (LUAJIT_DISABLE_SSE2)
      target_compile_options(libluajit PRIVATE -mno-sse2 -mfpmath=387)
    else()
      target_compile_options(libluajit PRIVATE -msse2 -mfpmath=sse)
    endif()
  endif()
endif()

if (MSVC)
  set(LJ_COMPILE_OPTIONS /fp:precise)
else()
  set(LJ_COMPILE_OPTIONS -fno-fast-math)
endif()

set(LJ_COMPILE_OPTIONS ${LJ_COMPILE_OPTIONS} -U_FORTIFY_SOURCE)

if (LJ_NO_STACK_PROTECTOR)
  set(LJ_COMPILE_OPTIONS ${LJ_COMPILE_OPTIONS} ${LJ_NO_STACK_PROTECTOR})
endif()
if (LJ_NO_STRICT_FLOAT_CAST_OVERFLOW)
  set(LJ_COMPILE_OPTIONS ${LJ_COMPILE_OPTIONS} ${LJ_NO_STRICT_FLOAT_CAST_OVERFLOW})
endif()

target_compile_options(libluajit PRIVATE ${LJ_COMPILE_OPTIONS})

set(luajit_headers
  ${LJ_DIR}/lauxlib.h
  ${LJ_DIR}/lua.h
  ${LJ_DIR}/lua.hpp
  ${LJ_DIR}/luaconf.h
  ${LJ_DIR}/lualib.h
  ${CMAKE_CURRENT_BINARY_DIR}/luajit.h)
install(FILES ${luajit_headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/luajit)
install(TARGETS libluajit
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Build the luajit binary
if (LUAJIT_BUILD_TOOL)
  add_executable(luajit ${LJ_DIR}/luajit.c)
  target_link_libraries(luajit libluajit)
  target_include_directories(luajit PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LJ_DIR}
  )
  if (APPLE AND LJ_64 AND NOT LUAJIT_ENABLE_GC64)
    target_link_libraries(luajit "-pagezero_size 10000" "-image_base 100000000")
  endif()
  if ("${CMAKE_SYSTEM_NAME}" MATCHES "(Open|Free|Net)BSD")
    target_link_libraries(luajit c++abi pthread)
  endif()

  target_compile_definitions(luajit PRIVATE ${LJ_DEFINITIONS})
  target_compile_options(luajit PRIVATE ${LJ_COMPILE_OPTIONS})

  install(TARGETS luajit DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
