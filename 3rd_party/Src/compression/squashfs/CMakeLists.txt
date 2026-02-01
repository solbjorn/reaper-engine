cmake_minimum_required(VERSION 3.18)
project(squashfs VERSION 5.0.4 LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(sqfs_headers
    squashfs-tools-ng/include/compat.h
    squashfs-tools-ng/include/sqfs/block.h
    squashfs-tools-ng/include/sqfs/block_processor.h
    squashfs-tools-ng/include/sqfs/block_writer.h
    squashfs-tools-ng/include/sqfs/compressor.h
    squashfs-tools-ng/include/sqfs/data_reader.h
    squashfs-tools-ng/include/sqfs/dir.h
    squashfs-tools-ng/include/sqfs/dir_entry.h
    squashfs-tools-ng/include/sqfs/dir_reader.h
    squashfs-tools-ng/include/sqfs/dir_writer.h
    squashfs-tools-ng/include/sqfs/error.h
    squashfs-tools-ng/include/sqfs/frag_table.h
    squashfs-tools-ng/include/sqfs/id_table.h
    squashfs-tools-ng/include/sqfs/inode.h
    squashfs-tools-ng/include/sqfs/io.h
    squashfs-tools-ng/include/sqfs/meta_reader.h
    squashfs-tools-ng/include/sqfs/meta_writer.h
    squashfs-tools-ng/include/sqfs/predef.h
    squashfs-tools-ng/include/sqfs/super.h
    squashfs-tools-ng/include/sqfs/table.h
    squashfs-tools-ng/include/sqfs/xattr.h
    squashfs-tools-ng/include/sqfs/xattr_reader.h
    squashfs-tools-ng/include/sqfs/xattr_writer.h
    squashfs-tools-ng/include/util/array.h
    squashfs-tools-ng/include/util/hash_table.h
    squashfs-tools-ng/include/util/mempool.h
    squashfs-tools-ng/include/util/rbtree.h
    squashfs-tools-ng/include/util/threadpool.h
)

set(sqfs_sources
    ${sqfs_headers}
    squashfs-tools-ng/lib/compat/src/strchrnul.c
    squashfs-tools-ng/lib/compat/src/strndup.c
    squashfs-tools-ng/lib/sqfs/src/block_processor/backend.c
    squashfs-tools-ng/lib/sqfs/src/block_processor/block_processor.c
    squashfs-tools-ng/lib/sqfs/src/block_processor/frontend.c
    squashfs-tools-ng/lib/sqfs/src/block_processor/internal.h
    squashfs-tools-ng/lib/sqfs/src/block_processor/ostream.c
    squashfs-tools-ng/lib/sqfs/src/block_writer.c
    squashfs-tools-ng/lib/sqfs/src/comp/compressor.c
    squashfs-tools-ng/lib/sqfs/src/comp/internal.h
    squashfs-tools-ng/lib/sqfs/src/comp/lz4.c
    squashfs-tools-ng/lib/sqfs/src/comp/zstd.c
    squashfs-tools-ng/lib/sqfs/src/data_reader.c
    squashfs-tools-ng/lib/sqfs/src/dir_entry.c
    squashfs-tools-ng/lib/sqfs/src/dir_iterator.c
    squashfs-tools-ng/lib/sqfs/src/dir_reader.c
    squashfs-tools-ng/lib/sqfs/src/dir_writer.c
    squashfs-tools-ng/lib/sqfs/src/frag_table.c
    squashfs-tools-ng/lib/sqfs/src/id_table.c
    squashfs-tools-ng/lib/sqfs/src/inode.c
    squashfs-tools-ng/lib/sqfs/src/io/dir_hl.c
    squashfs-tools-ng/lib/sqfs/src/io/dir_rec.c
    squashfs-tools-ng/lib/sqfs/src/io/dir_win32.c
    squashfs-tools-ng/lib/sqfs/src/io/file.c
    squashfs-tools-ng/lib/sqfs/src/io/istream.c
    squashfs-tools-ng/lib/sqfs/src/io/ostream.c
    squashfs-tools-ng/lib/sqfs/src/io/stream_api.c
    squashfs-tools-ng/lib/sqfs/src/io/win32.c
    squashfs-tools-ng/lib/sqfs/src/meta_reader.c
    squashfs-tools-ng/lib/sqfs/src/meta_writer.c
    squashfs-tools-ng/lib/sqfs/src/misc.c
    squashfs-tools-ng/lib/sqfs/src/read_inode.c
    squashfs-tools-ng/lib/sqfs/src/read_super.c
    squashfs-tools-ng/lib/sqfs/src/read_table.c
    squashfs-tools-ng/lib/sqfs/src/readdir.c
    squashfs-tools-ng/lib/sqfs/src/super.c
    squashfs-tools-ng/lib/sqfs/src/write_inode.c
    squashfs-tools-ng/lib/sqfs/src/write_super.c
    squashfs-tools-ng/lib/sqfs/src/write_table.c
    squashfs-tools-ng/lib/sqfs/src/xattr/xattr.c
    squashfs-tools-ng/lib/sqfs/src/xattr/xattr_reader.c
    squashfs-tools-ng/lib/sqfs/src/xattr/xattr_writer.c
    squashfs-tools-ng/lib/sqfs/src/xattr/xattr_writer.h
    squashfs-tools-ng/lib/sqfs/src/xattr/xattr_writer_flush.c
    squashfs-tools-ng/lib/sqfs/src/xattr/xattr_writer_record.c
    squashfs-tools-ng/lib/util/src/alloc.c
    squashfs-tools-ng/lib/util/src/array.c
    squashfs-tools-ng/lib/util/src/file_cmp.c
    squashfs-tools-ng/lib/util/src/hash_table.c
    squashfs-tools-ng/lib/util/src/is_memory_zero.c
    squashfs-tools-ng/lib/util/src/mempool.c
    squashfs-tools-ng/lib/util/src/rbtree.c
    squashfs-tools-ng/lib/util/src/str_table.c
    squashfs-tools-ng/lib/util/src/threadpool.c
    squashfs-tools-ng/lib/util/src/xxhash.c
)

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif()

if (BUILD_SHARED_LIBS)
    add_library(squashfs SHARED ${sqfs_sources})
else()
    add_library(squashfs STATIC ${sqfs_sources})
endif()

find_package(lz4 REQUIRED)
find_package(zstd REQUIRED)

target_include_directories(squashfs PRIVATE "${CMAKE_SOURCE_DIR}/inc")
target_include_directories(squashfs PRIVATE "${CMAKE_SOURCE_DIR}/squashfs-tools-ng/include")

target_link_libraries(squashfs PRIVATE lz4::lz4 zstd::libzstd)

set_target_properties(squashfs PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(squashfs PRIVATE WITH_LZ4 WITH_ZSTD)

if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(squashfs PRIVATE SQFS_STATIC)
endif()

target_compile_options(squashfs PRIVATE "$<$<CONFIG:Release>:/Zi>")
target_link_options(squashfs PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
target_link_options(squashfs PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
target_link_options(squashfs PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
