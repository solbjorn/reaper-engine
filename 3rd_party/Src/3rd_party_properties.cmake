set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

# https://github.com/llvm/llvm-project/commit/2ba7f11bce7f
if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
  set(CMAKE_PCH_PROLOGUE "")
endif()

set(conformance_options "/Brepro /bigobj /permissive- /volatile:iso /Zc:inline /Zc:preprocessor /Zc:enumTypes /Zc:lambda /Zc:__STDC__ /Zc:__cplusplus /Zc:externConstexpr /Zc:throwingNew /Zc:checkGwOdr /Zc:templateScope /DNOMINMAX -fno-delayed-template-parsing -fstrict-aliasing /D_CRT_STDIO_ISO_WIDE_SPECIFIERS -fno-ms-compatibility -fgnuc-version=0 /utf-8")

set(llvm_options "-march=skylake -mavx2 /FIintrin.h -flto -fmerge-all-constants -fforce-emit-vtables -fwhole-program-vtables /clang:-fcoro-aligned-allocation")

set(warning_options "-Wextra -Wmost -Wno-error=unused-command-line-argument -Werror=format -Wformat-nonliteral -Werror=format-pedantic -Werror=format-signedness -Werror=format-type-confusion -Werror=microsoft -Werror=move -Werror=nan-infinity-disabled -Werror=parentheses -Werror=strict-aliasing -Werror=tautological-compare -Werror=typename-missing -Werror=weak-vtables")

# DirectXTex
if(DEFINED BUILD_TOOLS)
  set(conformance_options "${conformance_options} -D_FORCENAMELESSUNION")
endif()

# SFML
if(SFML_USE_SYSTEM_DEPS)
  set(conformance_options "${conformance_options} -D_WIN32_WINDOWS=0x0A00")
endif()

# abseil
if(ABSL_PROPAGATE_CXX_STD)
  set(warning_options "${warning_options} -Wno-error=format -Wno-error=format-signedness")
endif()

# flac
if(DEFINED BUILD_CXXLIBS)
  set(conformance_options "${conformance_options} -DS_IWRITE=_S_IWRITE -Dfileno=_fileno -Dgetenv=getenv -Doff_t=_off_t -Dstrdup=_strdup -Dutimbuf=_utimbuf")
endif()

# freetype
if(FT_ENABLE_ERROR_STRINGS)
  set(conformance_options "${conformance_options} -DFT_CONFIG_OPTION_SUBPIXEL_RENDERING")
endif()

# harfbuzz
if(HB_HAVE_FREETYPE)
  set(warning_options "${warning_options} -Wno-error=microsoft-enum-value -Wno-error=microsoft-exception-spec")
endif()

# hwloc
if(HWLOC_SKIP_LSTOPO)
  set(conformance_options "${conformance_options} -DO_RDONLY=_O_RDONLY -DS_IFREG=_S_IFREG -Dfstat=_fstat -Dstat=_stat")
endif()

# llvm-libc, libc++
if(LLVM_ENABLE_RUNTIMES)
  set(conformance_options "${conformance_options} -DO_BINARY=_O_BINARY -DO_CREAT=_O_CREAT -DO_RDONLY=_O_RDONLY -DO_WRONLY=_O_WRONLY -Dfdopen=_fdopen -Dfileno=_fileno -Doff_t=_off_t")
else()
  set(llvm_options "${llvm_options} /clang:-fopenmp")
endif()

# luajit2
if(LUAJIT_DIR)
  set(warning_options "${warning_options} -Wno-error=format -Wno-error=format-pedantic -Wno-error=format-signedness -Wno-error=microsoft-enum-value")
endif()

# squashfs
if(EXISTS "${CMAKE_SOURCE_DIR}/squashfs-tools-ng")
  set(conformance_options "${conformance_options} -Dstrdup=_strdup")
endif()

# vorbis
if(CMAKE_POLICY_VERSION_MINIMUM)
  set(conformance_options "${conformance_options} -Dalloca=_alloca")
  set(warning_options "${warning_options} -Wno-error=incompatible-pointer-types")
endif()

# zstd
if(ZSTD_BUILD_TESTS)
  set(conformance_options "${conformance_options} -DS_IFREG=_S_IFREG -Dfileno=_fileno -Dutimbuf=_utimbuf -Dutime=_utime")
  set(warning_options "${warning_options} -Wno-format-signedness")
endif()

set(CMAKE_ASM_MASM_FLAGS_RELEASE ${CMAKE_ASM_MASM_FLAGS_RELEASE} -m64)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /Gy /Gw /W4 /GF /GS /Zi /GT ${conformance_options} ${llvm_options} ${warning_options} /D_WIN32_WINNT=0x0A00 /DWINVER=0x0A00 /D_MSVC_STL_HARDENING=1 /DNDEBUG /D_ITERATOR_DEBUG_LEVEL=0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /Gy /Gw /W4 /GF /GS /Zi /GT ${conformance_options} ${llvm_options} $ENV{LIBCXX_OPTIONS} ${warning_options} /D_WIN32_WINNT=0x0A00 /DWINVER=0x0A00 /D_MSVC_STL_HARDENING=1 /DNDEBUG /D_ITERATOR_DEBUG_LEVEL=0")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /Brepro /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG /opt:lldlto=3 /opt:lldltocgo=3")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /Brepro /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG /opt:lldlto=3 /opt:lldltocgo=3")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /Brepro /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG /opt:lldlto=3 /opt:lldltocgo=3")

set(CMAKE_ASM_MASM_FLAGS_DEBUG ${CMAKE_ASM_MASM_FLAGS_DEBUG} -m64)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /Gy /Gw /W4 /GF /GS /Zi /GT ${conformance_options} ${llvm_options} ${warning_options} /D_WIN32_WINNT=0x0A00 /DWINVER=0x0A00 /D_MSVC_STL_HARDENING=1 /D_ITERATOR_DEBUG_LEVEL=2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /Gy /Gw /W4 /GF /GS /Zi /GT ${conformance_options} ${llvm_options} $ENV{LIBCXX_OPTIONS} ${warning_options} /D_WIN32_WINNT=0x0A00 /DWINVER=0x0A00 /D_MSVC_STL_HARDENING=1 /D_ITERATOR_DEBUG_LEVEL=2")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /Brepro /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /Brepro /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /Brepro /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
