include(ExternalProject)

set(OpenFX_GIT_REPOSITORY "https://github.com/AcademySoftwareFoundation/openfx.git")
set(OpenFX_GIT_TAG "OFX_Release_1.5")

set(OpenFX_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS})

ExternalProject_Add(
    OpenFX
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenFX
    GIT_REPOSITORY ${OpenFX_GIT_REPOSITORY}
    GIT_TAG ${OpenFX_GIT_TAG}
    CMAKE_ARGS ${OpenFX_ARGS}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND}
        -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/OpenFX/src/OpenFX/include
        ${CMAKE_INSTALL_PREFIX}/include/OpenFX)
