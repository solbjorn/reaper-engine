#
# Try to find budouxc libraries and include paths.
# Once done this will define
#
# BUDOUXC_FOUND
# BUDOUXC_INCLUDE_DIR
# BUDOUXC_LIBRARY
#

find_path(BUDOUXC_INCLUDE_DIR budoux.h)
find_library(BUDOUXC_LIBRARY NAMES budouxc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(budouxc DEFAULT_MSG BUDOUXC_LIBRARY BUDOUXC_INCLUDE_DIR)

mark_as_advanced(BUDOUXC_INCLUDE_DIR BUDOUXC_LIBRARY)

add_library(budouxc::budouxc IMPORTED UNKNOWN)
set_target_properties(budouxc::budouxc PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${BUDOUXC_INCLUDE_DIR})

if(BUDOUXC_LIBRARY MATCHES "/([^/]+)\\.framework$")
	set_target_properties(budouxc::budouxc PROPERTIES IMPORTED_LOCATION ${BUDOUXC_LIBRARY}/${CMAKE_MATCH_1})
else()
	set_target_properties(budouxc::budouxc PROPERTIES IMPORTED_LOCATION ${BUDOUXC_LIBRARY})
endif()
