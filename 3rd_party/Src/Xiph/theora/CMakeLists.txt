cmake_minimum_required(VERSION 3.18)
project(theora VERSION 1.1.1 LANGUAGES C)

if(MSVC)
    set(theora_arch_sources
        theora/lib/x86_vc/mmxencfrag.c
        theora/lib/x86_vc/mmxfdct.c
        theora/lib/x86_vc/mmxfrag.c
        theora/lib/x86_vc/mmxidct.c
        theora/lib/x86_vc/mmxstate.c
        theora/lib/x86_vc/x86cpu.c
        theora/lib/x86_vc/x86enc.c
        theora/lib/x86_vc/x86state.c
    )
else()
    set(theora_arch_sources
        theora/lib/x86/mmxencfrag.c
        theora/lib/x86/mmxfdct.c
        theora/lib/x86/mmxfrag.c
        theora/lib/x86/mmxidct.c
        theora/lib/x86/mmxstate.c
        theora/lib/x86/sse2encfrag.c
        theora/lib/x86/sse2fdct.c
        theora/lib/x86/sse2idct.c
        theora/lib/x86/x86cpu.c
        theora/lib/x86/x86enc.c
        theora/lib/x86/x86enquant.c
        theora/lib/x86/x86state.c
    )
endif()

set(theora_sources
    ${theora_arch_sources}
    theora/lib/analyze.c
    theora/lib/apiwrapper.c
    theora/lib/bitpack.c
    theora/lib/collect.c
    theora/lib/decapiwrapper.c
    theora/lib/decinfo.c
    theora/lib/decode.c
    theora/lib/dequant.c
    theora/lib/encapiwrapper.c
    theora/lib/encfrag.c
    theora/lib/encinfo.c
    theora/lib/encode.c
    theora/lib/encoder_disabled.c
    theora/lib/enquant.c
    theora/lib/fdct.c
    theora/lib/fragment.c
    theora/lib/huffdec.c
    theora/lib/huffenc.c
    theora/lib/idct.c
    theora/lib/info.c
    theora/lib/internal.c
    theora/lib/mathops.c
    theora/lib/mcenc.c
    theora/lib/quant.c
    theora/lib/rate.c
    theora/lib/state.c
    theora/lib/tokenize.c
)

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF)
endif()

if(BUILD_SHARED_LIBS)
    add_library(theora SHARED ${theora_sources})
else()
    add_library(theora STATIC ${theora_sources})
endif()

if(NOT DEFINED OGG_ROOT)
    message(FATAL_ERROR "OGG_ROOT needs to be set to access ogg headers")
endif()

target_include_directories(theora PRIVATE "${CMAKE_SOURCE_DIR}/theora/include")
target_include_directories(theora PRIVATE "${OGG_ROOT}/include")

set_target_properties(theora PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(theora
    PRIVATE
    _BIND_TO_CURRENT_CRT_VERSION
    _CRT_NONSTDC_NO_DEPRECATE
    _CRT_SECURE_NO_DEPRECATE
    _USRDLL
    _WINDOWS
    WIN32
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(theora PRIVATE LIBTHEORA_EXPORTS)
endif()

target_compile_definitions(theora PRIVATE "$<$<CONFIG:Release>:NDEBUG>")
target_compile_definitions(theora PRIVATE "$<$<CONFIG:Debug>:_DEBUG>")
target_compile_definitions(theora PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

target_compile_options(theora PRIVATE "$<$<CONFIG:Release>:/Zi>")
target_link_options(theora PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
target_link_options(theora PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
target_link_options(theora PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
