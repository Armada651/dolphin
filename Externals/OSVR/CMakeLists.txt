# OSVR warnings are not our problem.
add_definitions(-w)

# OSVR needs exception handling.
add_definitions( -fexceptions )

# Disable OSVR unit tests.
set(BUILD_TESTING FALSE CACHE INTERNAL "" FORCE)

# Don't build the OSVR server.
set(BUILD_SERVER FALSE CACHE INTERNAL "" FORCE)
set(BUILD_SERVER_APP FALSE CACHE INTERNAL "" FORCE)
set(BUILD_SERVER_PLUGINS FALSE CACHE INTERNAL "" FORCE)

# Build the OSVR client without example applications.
set(BUILD_CLIENT TRUE CACHE INTERNAL "" FORCE)
set(BUILD_CLIENT_APPS FALSE CACHE INTERNAL "" FORCE)

add_subdirectory(libfunctionality)
add_subdirectory(OSVR-Core)
