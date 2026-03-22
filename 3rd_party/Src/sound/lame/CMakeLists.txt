cmake_minimum_required(VERSION 3.22)

project(lame
	VERSION 3.101
	DESCRIPTION "A high quality MPEG Audio Layer III (MP3) encoder"
	HOMEPAGE_URL "https://lame.sourceforge.io"
	LANGUAGES C
)

set(CMAKE_C_STANDARD 23)

include(CheckTypeSize)
include(GNUInstallDirs)

check_type_size("long double" SIZEOF_LONG_DOUBLE
	BUILTIN_TYPES_ONLY
	LANGUAGE C
)

file(READ "lame/configMS.h" LAME_CONFIG_H_CONTENTS)

string(REPLACE
	"#define SIZEOF_LONG_DOUBLE 12"
	"#define SIZEOF_LONG_DOUBLE ${SIZEOF_LONG_DOUBLE}"
	LAME_CONFIG_H
	"${LAME_CONFIG_H_CONTENTS}"
)
set(LAME_CONFIG_H_CONTENTS "${LAME_CONFIG_H}")

string(REPLACE
	"#if defined(_MSC_VER) || defined(__BORLANDC__)"
	"#ifdef __BORLANDC__"
	LAME_CONFIG_H
	"${LAME_CONFIG_H_CONTENTS}"
)
set(LAME_CONFIG_H_CONTENTS "${LAME_CONFIG_H}")

string(REPLACE
	"#ifdef HAVE_MPGLIB"
	"#ifdef HAVE_MPG123"
	LAME_CONFIG_H
	"${LAME_CONFIG_H_CONTENTS}"
)
set(LAME_CONFIG_H_CONTENTS "${LAME_CONFIG_H}")

set(LAME_CONFIG_H_NEEDS_UPDATE TRUE)

if(EXISTS "${CMAKE_BINARY_DIR}/config.h")
	file(READ "${CMAKE_BINARY_DIR}/config.h" LAME_CONFIG_H_CONTENTS)

	if("${LAME_CONFIG_H}" STREQUAL "${LAME_CONFIG_H_CONTENTS}")
		set(LAME_CONFIG_H_NEEDS_UPDATE FALSE)
	endif()
endif()

if(LAME_CONFIG_H_NEEDS_UPDATE)
	file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/config.h" CONTENT "${LAME_CONFIG_H}")
endif()

add_library(mp3lame STATIC)
set(SRC lame/libmp3lame)

target_sources(mp3lame PRIVATE
	${CMAKE_BINARY_DIR}/config.h
	lame/include/lame.h
	${SRC}/VbrTag.c
	${SRC}/VbrTag.h
	${SRC}/bitstream.c
	${SRC}/bitstream.h
	${SRC}/encoder.c
	${SRC}/encoder.h
	${SRC}/fft.c
	${SRC}/fft.h
	${SRC}/gain_analysis.c
	${SRC}/gain_analysis.h
	${SRC}/id3tag.c
	${SRC}/id3tag.h
	${SRC}/l3side.h
	${SRC}/lame-analysis.h
	${SRC}/lame.c
	${SRC}/lame_global_flags.h
	${SRC}/lameerror.h
	${SRC}/machine.h
	${SRC}/mpglib_interface.c
	${SRC}/newmdct.c
	${SRC}/newmdct.h
	${SRC}/presets.c
	${SRC}/psymodel.c
	${SRC}/psymodel.h
	${SRC}/quantize.c
	${SRC}/quantize.h
	${SRC}/quantize_pvt.c
	${SRC}/quantize_pvt.h
	${SRC}/reservoir.c
	${SRC}/reservoir.h
	${SRC}/set_get.c
	${SRC}/set_get.h
	${SRC}/tables.c
	${SRC}/tables.h
	${SRC}/takehiro.c
	${SRC}/util.c
	${SRC}/util.h
	${SRC}/vbrquantize.c
	${SRC}/vbrquantize.h
	${SRC}/vector/lame_intrin.h
	${SRC}/vector/xmm_quantize_sub.c
	${SRC}/version.c
	${SRC}/version.h
)

target_include_directories(mp3lame PUBLIC
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lame>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lame/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SRC}>
)

target_compile_definitions(mp3lame PRIVATE
	DECODE_ON_THE_FLY
	HAVE_CONFIG_H
	HAVE_MPG123
	HAVE_STDINT_H
	HAVE_XMMINTRIN_H
	MIN_ARCH_SSE
)

if(MSVC)
	target_compile_options(mp3lame PRIVATE /arch:AVX2 /O2 /Ob2)
else()
	target_compile_options(mp3lame PRIVATE -mavx2 -O3)
endif()

set_target_properties(mp3lame PROPERTIES
	OUTPUT_NAME mp3lame
	POSITION_INDEPENDENT_CODE ON
)

find_package(MPG123 REQUIRED)
target_link_libraries(mp3lame PRIVATE MPG123::libmpg123)

install(TARGETS mp3lame
	EXPORT mp3lame-config
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT mp3lame-config
	NAMESPACE mp3lame::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/mp3lame"
)

install(FILES
	lame/include/lame.h
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lame"
)
