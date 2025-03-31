set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-error=unused-command-line-argument /Zc:__cplusplus /MP /O2 /Ob2 /Oi /Oy /Ot /GL -march=haswell -mavx2 -mtune=native -flto -fmerge-all-constants -fwhole-program-vtables /W4 /GF /GS- /Zi /GT /D_WIN32_WINNT=0x0A00 -Xclang -fopenmp -Xclang -stdlib=libc++ /DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-error=unused-command-line-argument /Zc:__cplusplus /MP /O2 /Ob2 /Oi /Oy /Ot /GL -march=haswell -mavx2 -mtune=native -flto -fmerge-all-constants -fwhole-program-vtables /W4 /GF /GS- /Zi /GT /D_WIN32_WINNT=0x0A00 -Xclang -fopenmp -Xclang -stdlib=libc++ /DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /RELEASE /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wno-error=unused-command-line-argument /Zc:__cplusplus /MP /O2 /Ob2 /Oi /Oy /Ot /GL -march=haswell -mavx2 -mtune=native -flto -fmerge-all-constants -fwhole-program-vtables /W4 /GF /GS- /Zi /GT /D_WIN32_WINNT=0x0A00 -Xclang -stdlib=libc++")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-error=unused-command-line-argument /Zc:__cplusplus /MP /O2 /Ob2 /Oi /Oy /Ot /GL -march=haswell -mavx2 -mtune=native -flto -fmerge-all-constants -fwhole-program-vtables /W4 /GF /GS- /Zi /GT /D_WIN32_WINNT=0x0A00 -Xclang -stdlib=libc++")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG")
