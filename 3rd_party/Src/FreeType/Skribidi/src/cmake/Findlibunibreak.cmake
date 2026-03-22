#
# Try to find libunibreak libraries and include paths.
# Once done this will define
#
# LIBUNIBREAK_FOUND
# LIBUNIBREAK_INCLUDE_DIR
# LIBUNIBREAK_LIBRARY
#

find_path(LIBUNIBREAK_INCLUDE_DIR unibreakdef.h)
find_library(LIBUNIBREAK_LIBRARY NAMES unibreak)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libunibreak DEFAULT_MSG LIBUNIBREAK_LIBRARY LIBUNIBREAK_INCLUDE_DIR)

mark_as_advanced(LIBUNIBREAK_INCLUDE_DIR LIBUNIBREAK_LIBRARY)

add_library(libunibreak::libunibreak IMPORTED UNKNOWN)
set_target_properties(libunibreak::libunibreak PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBUNIBREAK_INCLUDE_DIR})

if(LIBUNIBREAK_LIBRARY MATCHES "/([^/]+)\\.framework$")
	set_target_properties(libunibreak::libunibreak PROPERTIES IMPORTED_LOCATION ${LIBUNIBREAK_LIBRARY}/${CMAKE_MATCH_1})
else()
	set_target_properties(libunibreak::libunibreak PROPERTIES IMPORTED_LOCATION ${LIBUNIBREAK_LIBRARY})
endif()
