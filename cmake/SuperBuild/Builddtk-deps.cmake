include(ExternalProject)

set(dtk_GIT_REPOSITORY "https://github.com/darbyjohnston/dtk.git")
set(dtk_GIT_TAG "3f569280b3ea1c9cf37241f80be7847b6105d12c")

set(dtk-deps_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -Ddtk_ZLIB=OFF
    -Ddtk_PNG=OFF
    -Ddtk_Freetype=OFF
    -Ddtk_DEPS_ONLY=ON)

ExternalProject_Add(
    dtk-deps
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/dtk-deps
    DEPENDS ZLIB PNG Freetype
    GIT_REPOSITORY ${dtk_GIT_REPOSITORY}
    GIT_TAG ${dtk_GIT_TAG}
    SOURCE_SUBDIR etc/SuperBuild
    INSTALL_COMMAND ""
    LIST_SEPARATOR |
    CMAKE_ARGS ${dtk-deps_ARGS})
