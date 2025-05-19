LIST(APPEND CMAKE_PROGRAM_PATH "$ENV{LLVMInstallDir}/bin/")

set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_C_LINKER lld-link)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_CXX_LINKER lld-link)
set(CMAKE_MT llvm-mt)
