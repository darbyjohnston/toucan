# Find the OpenTimelineIO library.
#
# This module defines the following variables:
#
# * OpenFX_INCLUDE_DIRS
#
# This module defines the following imported targets:
#
# * OpenFX::OpenFX
#
# This module defines the following interfaces:
#
# * OpenFX

find_path(OpenFX_INCLUDE_DIR NAMES OpenFX/ofxCore.h)
set(OpenFX_INCLUDE_DIRS ${OpenFX_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenFX
    REQUIRED_VARS OpenFX_INCLUDE_DIRS)
mark_as_advanced(OpenFX_INCLUDE_DIR)

if(OpenFX_FOUND AND NOT TARGET OpenFX::OpenFX)
    add_library(OpenFX::OpenFX UNKNOWN IMPORTED)
    set_target_properties(OpenFX::OpenFX PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS OpenFX_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${OpenFX_INCLUDE_DIR}")
endif()
if(OpenFX_FOUND AND NOT TARGET OpenFX)
    add_library(OpenFX INTERFACE)
    target_link_libraries(OpenFX INTERFACE OpenFX::OpenFX)
endif()
