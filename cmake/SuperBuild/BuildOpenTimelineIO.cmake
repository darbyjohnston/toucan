include(ExternalProject)

find_package(Git REQUIRED)

set(OpenTimelineIO_GIT_REPOSITORY "https://github.com/AcademySoftwareFoundation/OpenTimelineIO.git")
# "Add core C++ support for otioz and otiod, take 2 (#2021)", which also adds
# bundle support for multiple media references and image sequences. Newer than
# v0.18.1, which does not have it.
set(OpenTimelineIO_GIT_TAG "0eebd211b2055f111e2c53d04b5581adc594c1fc")

set(OpenTimelineIO_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -DOTIO_SHARED_LIBS=${BUILD_SHARED_LIBS}
    -DOTIO_FIND_IMATH=ON
    # Use the minizip-ng and zlib from the super build; without this OTIO
    # builds its own copies of both for its otioz and otiod support. This
    # depends on the patch below.
    -DOTIO_FIND_MINIZIP_NG=ON)

# OTIO is patched, with two changes; see the notes in the patch itself.
#
# The first has it link whichever minizip-ng target is present rather than
# assuming the one from the compatibility layer. Without it OTIO cannot be
# built against the super build's minizip-ng, which is built without that
# layer.
#
# The second is what a shared build on Windows needs: the export macros become
# PRIVATE and the members that lacked OTIO_API get it. A static build does not
# need it, and it is carried unchanged so that the patch stays the one
# tlRender uses.
#
# It goes away once these are upstream.
ExternalProject_Add(
    OpenTimelineIO
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenTimelineIO
    DEPENDS Imath minizip-ng
    GIT_REPOSITORY ${OpenTimelineIO_GIT_REPOSITORY}
    GIT_TAG ${OpenTimelineIO_GIT_TAG}
    PATCH_COMMAND ${CMAKE_COMMAND}
        -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
        -DOTIO_SOURCE_DIR=${CMAKE_CURRENT_BINARY_DIR}/OpenTimelineIO/src/OpenTimelineIO
        -DOTIO_PATCH=${CMAKE_CURRENT_SOURCE_DIR}/OTIO-patch/otio.patch
        -P ${CMAKE_CURRENT_LIST_DIR}/OTIOApplyPatch.cmake
    LIST_SEPARATOR |
    CMAKE_ARGS ${OpenTimelineIO_ARGS})
