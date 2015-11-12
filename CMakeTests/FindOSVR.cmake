# - Find OSVR library
# This module defines
#  OSVR_INCLUDE_DIR, include location for OSVR headers
#  OSVR_LIBRARIES, link location for OSVR libs
#  OSVR_FOUND, If false, do not try to use OSVR.

if (NOT OSVR_FOUND)

	find_path(OSVR_INCLUDE_DIR NAMES ClientKitC.h
		PATHS
		/usr/include
		/usr/include/osvr
		/usr/include/osvr/ClientKit
		Externals/OSVR/inc/osvr/ClientKit
	)

	find_library(OSVR_LIBRARIES NAMES osvrClientKit
		PATHS
		/lib
		/usr/lib
		Externals/OSVR/lib64
	)

	if(OSVR_INCLUDE_DIR AND OSVR_LIBRARIES)
		set(OSVR_FOUND TRUE CACHE INTERNAL "OSVR Found")
		message(STATUS "Found OSVR: ${OSVR_INCLUDE_DIR}")
	else()
		set(OSVR_FOUND FALSE CACHE INTERNAL "OSVR found")
		message(STATUS "OSVR not found.")
	endif(OSVR_INCLUDE_DIR AND OSVR_LIBRARIES)

	mark_as_advanced(OSVR_INCLUDE_DIR AND OSVR_LIBRARIES)
endif ()
