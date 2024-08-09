include(ExternalProject)

set(OpenEXR_GIT_REPOSITORY "https://github.com/AcademySoftwareFoundation/openexr.git")
set(OpenEXR_GIT_TAG "v3.2.0")

set(OpenEXR_ARGS
    -DOPENEXR_BUILD_TOOLS=OFF
    -DOPENEXR_INSTALL_EXAMPLES=OFF
    -DBUILD_TESTING=OFF
    -DOPENEXR_FORCE_INTERNAL_DEFLATE=ON
    ${toucan_EXTERNAL_PROJECT_ARGS})

ExternalProject_Add(
    OpenEXR
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenEXR
    DEPENDS Imath ZLIB
    GIT_REPOSITORY ${OpenEXR_GIT_REPOSITORY}
    GIT_TAG ${OpenEXR_GIT_TAG}
    CMAKE_ARGS ${OpenEXR_ARGS})
