cmake_minimum_required(VERSION 3.30)
project(theora LANGUAGES C)

enable_testing()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
find_package(Ogg CONFIG REQUIRED)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/theora")

file(GLOB HEADERS
  "${SRC}/include/theora/codec.h"
  "${SRC}/include/theora/theora.h"
  "${SRC}/include/theora/theoradec.h"
  "${SRC}/include/theora/theoraenc.h"
)

if(MSVC AND NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(LIBTHEORA_COMMON_X86
    "${SRC}/lib/x86_vc/mmxfrag.c"
    "${SRC}/lib/x86_vc/mmxidct.c"
    "${SRC}/lib/x86_vc/mmxstate.c"
    "${SRC}/lib/x86_vc/x86cpu.c"
    "${SRC}/lib/x86_vc/x86state.c"
  )
else()
  set(LIBTHEORA_COMMON_X86
    "${SRC}/lib/x86/mmxfrag.c"
    "${SRC}/lib/x86/mmxidct.c"
    "${SRC}/lib/x86/mmxstate.c"
    "${SRC}/lib/x86/sse2idct.c"
    "${SRC}/lib/x86/x86cpu.c"
    "${SRC}/lib/x86/x86state.c"
  )
endif()

set(LIBTHEORA_COMMON
  "${SRC}/lib/apiwrapper.c"
  "${SRC}/lib/bitpack.c"
  "${SRC}/lib/dequant.c"
  "${SRC}/lib/fragment.c"
  "${SRC}/lib/idct.c"
  "${SRC}/lib/info.c"
  "${SRC}/lib/internal.c"
  "${SRC}/lib/state.c"
  "${SRC}/lib/quant.c"

  ${LIBTHEORA_COMMON_X86}
)

if(MSVC AND NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(LIBTHEORA_ENC_X86
    "${SRC}/lib/x86_vc/mmxencfrag.c"
    "${SRC}/lib/x86_vc/mmxfdct.c"
    "${SRC}/lib/x86_vc/x86enc.c"
  )
else()
  set(LIBTHEORA_ENC_X86
    "${SRC}/lib/x86/mmxencfrag.c"
    "${SRC}/lib/x86/mmxfdct.c"
    "${SRC}/lib/x86/x86enc.c"
    "${SRC}/lib/x86/x86enquant.c"
    "${SRC}/lib/x86/sse2encfrag.c"
  )
endif()

set(LIBTHEORA_ENC
  "${SRC}/lib/analyze.c"
  "${SRC}/lib/encapiwrapper.c"
  "${SRC}/lib/encfrag.c"
  "${SRC}/lib/encinfo.c"
  "${SRC}/lib/encode.c"
  "${SRC}/lib/enquant.c"
  "${SRC}/lib/fdct.c"
  "${SRC}/lib/huffenc.c"
  "${SRC}/lib/mathops.c"
  "${SRC}/lib/mcenc.c"
  "${SRC}/lib/rate.c"
  "${SRC}/lib/tokenize.c"

  ${LIBTHEORA_ENC_X86}
)

set(LIBTHEORA_DEC
  "${SRC}/lib/decapiwrapper.c"
  "${SRC}/lib/decinfo.c"
  "${SRC}/lib/decode.c"
  "${SRC}/lib/huffdec.c"
)

if(MSVC)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

option(USE_X86 "Use x86 optimization" OFF)
if(USE_X86)
  add_definitions(-DOC_X86_ASM)
  add_definitions(-DOC_X86_64_ASM)
endif()

if(BUILD_SHARED_LIBS)
  add_definitions(-DLIBTHEORA_EXPORTS)
endif()

add_library(theora-common OBJECT ${LIBTHEORA_COMMON} ${HEADERS})
target_link_libraries(theora-common PUBLIC Ogg::ogg)
target_include_directories(theora-common PUBLIC $<BUILD_INTERFACE:${SRC}/include> $<INSTALL_INTERFACE:include>)
add_library(theora-enc OBJECT ${LIBTHEORA_ENC} ${HEADERS})
target_link_libraries(theora-enc PUBLIC Ogg::ogg)
target_include_directories(theora-enc PUBLIC $<BUILD_INTERFACE:${SRC}/include> $<INSTALL_INTERFACE:include>)
add_library(theora-dec OBJECT ${LIBTHEORA_DEC} ${HEADERS})
target_link_libraries(theora-dec PUBLIC Ogg::ogg)
target_include_directories(theora-dec PUBLIC $<BUILD_INTERFACE:${SRC}/include> $<INSTALL_INTERFACE:include>)

add_library(theora $<TARGET_OBJECTS:theora-common> $<TARGET_OBJECTS:theora-enc> $<TARGET_OBJECTS:theora-dec> "cmake/libtheora.def")
target_link_libraries(theora PUBLIC Ogg::ogg)
target_include_directories(theora PUBLIC $<BUILD_INTERFACE:${SRC}/include> $<INSTALL_INTERFACE:include>)

add_library(theoraenc $<TARGET_OBJECTS:theora-common> $<TARGET_OBJECTS:theora-enc> "${SRC}/win32/xmingw32/libtheoraenc-all.def")
target_link_libraries(theoraenc PUBLIC Ogg::ogg)
target_include_directories(theoraenc PUBLIC $<BUILD_INTERFACE:${SRC}/include> $<INSTALL_INTERFACE:include>)

add_library(theoradec $<TARGET_OBJECTS:theora-common> $<TARGET_OBJECTS:theora-dec> "${SRC}/win32/xmingw32/libtheoradec-all.def")
target_link_libraries(theoradec PUBLIC Ogg::ogg)
target_include_directories(theoradec PUBLIC $<BUILD_INTERFACE:${SRC}/include> $<INSTALL_INTERFACE:include>)

include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/theora-config.cmake.in theora-config.cmake
  INSTALL_DESTINATION "lib/cmake/theora")

install(FILES ${HEADERS} DESTINATION include/theora)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/theora-config.cmake"
  DESTINATION "lib/cmake/theora"
)

install(TARGETS theora theoraenc theoradec
  EXPORT theora-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION bin
  ARCHIVE DESTINATION lib
)

install(EXPORT theora-targets
  NAMESPACE theora::
  DESTINATION "lib/cmake/theora"
)
