cmake_minimum_required(VERSION 3.22)

project(libunibreak
	VERSION 7.0
	DESCRIPTION "The libunibreak library"
	HOMEPAGE_URL "https://github.com/adah1972/libunibreak"
	LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
include(GNUInstallDirs)

add_library(libunibreak STATIC)

set(SRC libunibreak/src)

target_sources(libunibreak PRIVATE
	${SRC}/eastasianwidthdata.c
	${SRC}/eastasianwidthdef.c
	${SRC}/eastasianwidthdef.h
	${SRC}/emojidata.c
	${SRC}/emojidef.c
	${SRC}/emojidef.h
	${SRC}/graphemebreak.c
	${SRC}/graphemebreak.h
	${SRC}/graphemebreakdata.c
	${SRC}/graphemebreakdef.h
	${SRC}/indicconjunctbreakdata.c
	${SRC}/indicconjunctbreakdef.h
	${SRC}/linebreak.c
	${SRC}/linebreak.h
	${SRC}/linebreakdata.c
	${SRC}/linebreakdef.c
	${SRC}/linebreakdef.h
	${SRC}/unibreakbase.c
	${SRC}/unibreakbase.h
	${SRC}/unibreakdef.c
	${SRC}/unibreakdef.h
	${SRC}/wordbreak.c
	${SRC}/wordbreak.h
	${SRC}/wordbreakdata.c
	${SRC}/wordbreakdef.h
)

target_include_directories(libunibreak PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SRC}>)

set_target_properties(libunibreak PROPERTIES
	OUTPUT_NAME unibreak
	POSITION_INDEPENDENT_CODE ON
)

install(TARGETS libunibreak
	EXPORT libunibreakConfig
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT libunibreakConfig
	NAMESPACE libunibreak::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libunibreak"
)

install(FILES
	${SRC}/eastasianwidthdef.h
	${SRC}/graphemebreak.h
	${SRC}/linebreak.h
	${SRC}/linebreakdef.h
	${SRC}/unibreakbase.h
	${SRC}/unibreakdef.h
	${SRC}/wordbreak.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
