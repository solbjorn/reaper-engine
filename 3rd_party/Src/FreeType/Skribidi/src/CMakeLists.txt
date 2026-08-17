list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(HarfBuzz REQUIRED)
find_package(SheenBidi REQUIRED)
find_package(budouxc REQUIRED)
find_package(libunibreak REQUIRED)

set(SRC ../Skribidi/src)

#
# Project files
#
set(SKRIBIDI_SOURCE_FILES
	${SRC}/emoji_data.h
	${SRC}/skb_attributes.c
	${SRC}/skb_attribute_collection.c
	${SRC}/skb_canvas.c
	${SRC}/skb_common.c
	${SRC}/skb_common_internal.h
	${SRC}/skb_editor.c
	${SRC}/skb_editor_rules.c
	${SRC}/skb_font_collection.c
	${SRC}/skb_font_collection_internal.h
	${SRC}/skb_icon_collection.c
	${SRC}/skb_icon_collection_internal.h
	${SRC}/skb_image_atlas.c
	${SRC}/skb_layout.c
	${SRC}/skb_layout_internal.h
	${SRC}/skb_layout_cache.c
	${SRC}/skb_rasterizer.c
	${SRC}/skb_rich_layout.c
	${SRC}/skb_rich_layout_internal.h
	${SRC}/skb_rich_text.c
	${SRC}/skb_rich_text_internal.h
	${SRC}/skb_text.c
	${SRC}/skb_text_internal.h
)

set(SKRIBIDI_API_FILES
	${SRC}/../include/skribidi/skb_attributes.h
	${SRC}/../include/skribidi/skb_attribute_collection.h
	${SRC}/../include/skribidi/skb_canvas.h
	${SRC}/../include/skribidi/skb_common.h
	${SRC}/../include/skribidi/skb_editor.h
	${SRC}/../include/skribidi/skb_editor_rules.h
	${SRC}/../include/skribidi/skb_font_collection.h
	${SRC}/../include/skribidi/skb_icon_collection.h
	${SRC}/../include/skribidi/skb_image_atlas.h
	${SRC}/../include/skribidi/skb_layout.h
	${SRC}/../include/skribidi/skb_layout_cache.h
	${SRC}/../include/skribidi/skb_rasterizer.h
	${SRC}/../include/skribidi/skb_rich_layout.h
	${SRC}/../include/skribidi/skb_rich_text.h
	${SRC}/../include/skribidi/skb_text.h
)

add_library(skribidi ${SKRIBIDI_LIBRARY_TYPE} ${SKRIBIDI_SOURCE_FILES} ${SKRIBIDI_API_FILES})

target_include_directories(skribidi
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${SRC}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(skribidi PRIVATE "${BUDOUXC_INCLUDE_DIR}/include")

set(CMAKE_DEBUG_POSTFIX "d")

set_target_properties(skribidi PROPERTIES
	C_STANDARD 17
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
	C_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN YES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

if(NOT WIN32)
	target_link_libraries(skribidi PUBLIC m)
endif()

target_link_libraries(skribidi PRIVATE HarfBuzz::HarfBuzz SheenBidi::SheenBidi budouxc::budouxc libunibreak::libunibreak)

if (MSVC)
	target_sources(skribidi PRIVATE ${SRC}/skribidi.natvis)
endif()

if (SKRIBIDI_BUILD_SHARED_LIBRARY)
	target_compile_definitions(skribidi PRIVATE SKB_BUILD_DLL)

	if (WIN32)
		target_compile_definitions(skribidi INTERFACE SKB_DLL)
	endif()
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}" PREFIX "src" FILES ${SKRIBIDI_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}/../include" PREFIX "include" FILES ${SKRIBIDI_API_FILES})

install(
  TARGETS skribidi
  EXPORT skribidiConfig
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  EXPORT skribidiConfig
  NAMESPACE skribidi::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/skribidi"
)

install(
  FILES ${SKRIBIDI_API_FILES}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/skribidi
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/skribidiConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/skribidiConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/skribidi"
)
