set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(conformance_options "/bigobj /permissive- /volatile:iso /Zc:inline /Zc:preprocessor /Zc:enumTypes /Zc:lambda /Zc:__cplusplus /Zc:externConstexpr /Zc:throwingNew /DNOMINMAX -fno-delayed-template-parsing -fstrict-aliasing /D_CRT_STDIO_ISO_WIDE_SPECIFIERS -fno-ms-compatibility -fgnuc-version=0")

if(NOT LLVM_ENABLE_RUNTIMES)
  set(llvm_options "-march=haswell -mavx2 -flto -fmerge-all-constants -fwhole-program-vtables -Xclang -fopenmp")
else()
  set(llvm_options "-march=haswell -mavx2 -flto -fmerge-all-constants -fwhole-program-vtables")
endif()

set(warning_options "-Wextra -Wno-error=unused-command-line-argument -Werror=microsoft -Werror=strict-aliasing -Werror=typename-missing")

# hwloc
if(HWLOC_SKIP_LSTOPO)
  set(conformance_options "${conformance_options} -DO_RDONLY=_O_RDONLY -DS_IFREG=_S_IFREG -Dfstat=_fstat -Dstat=_stat")
endif()

# libsquashfs
if(LZ4_INC)
  set(conformance_options "${conformance_options} -Dstrdup=_strdup")
endif()

# llvm-libc, libc++
if(LLVM_ENABLE_RUNTIMES)
  set(conformance_options "${conformance_options} -DO_BINARY=_O_BINARY -DO_CREAT=_O_CREAT -DO_RDONLY=_O_RDONLY -DO_WRONLY=_O_WRONLY -Dfdopen=_fdopen -Dfileno=_fileno -Doff_t=_off_t")
endif()

# luajit2
if(LUAJIT_DIR)
  set(warning_options "${warning_options} -Wno-error=microsoft-enum-value")
endif()

# ogg
if(OGG_ROOT)
  set(conformance_options "${conformance_options} -Dalloca=_alloca")
endif()

# oneTBB
if(TBB_VERIFY_DEPENDENCY_SIGNATURE)
  set(warning_options "${warning_options} -Wno-error=microsoft-enum-value")
endif()

# zstd
if(ZSTD_BUILD_TESTS)
  set(conformance_options "${conformance_options} -DS_IFREG=_S_IFREG -Dfileno=_fileno -Dutimbuf=_utimbuf -Dutime=_utime")
endif()

set(CMAKE_ASM_MASM_FLAGS_RELEASE ${CMAKE_ASM_MASM_FLAGS_RELEASE} -m64)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /W4 /GF /GS- /Zi /GT ${conformance_options} ${llvm_options} ${warning_options} /D_WIN32_WINNT=0x0A00 /D_MSVC_STL_HARDENING=1 /DNDEBUG /D_ITERATOR_DEBUG_LEVEL=0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /W4 /GF /GS- /Zi /GT ${conformance_options} ${llvm_options} $ENV{LIBCXX_OPTIONS} ${warning_options} /D_WIN32_WINNT=0x0A00 /D_MSVC_STL_HARDENING=1 /DNDEBUG /D_ITERATOR_DEBUG_LEVEL=0")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG /opt:lldlto=3 /opt:lldltocgo=3")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG /opt:lldlto=3 /opt:lldltocgo=3")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG /opt:lldlto=3 /opt:lldltocgo=3")

set(CMAKE_ASM_MASM_FLAGS_DEBUG ${CMAKE_ASM_MASM_FLAGS_DEBUG} -m64)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /W4 /GF /GS- /Zi /GT ${conformance_options} ${llvm_options} ${warning_options} /D_WIN32_WINNT=0x0A00 /D_MSVC_STL_HARDENING=1 /D_ITERATOR_DEBUG_LEVEL=2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP /O2 /Ob2 /Oi /Oy /Ot /GL /W4 /GF /GS- /Zi /GT ${conformance_options} ${llvm_options} $ENV{LIBCXX_OPTIONS} ${warning_options} /D_WIN32_WINNT=0x0A00 /D_MSVC_STL_HARDENING=1 /D_ITERATOR_DEBUG_LEVEL=2")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
