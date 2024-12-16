include(ExternalProject)

set(dtk_GIT_REPOSITORY "https://github.com/darbyjohnston/dtk.git")
set(dtk_GIT_TAG "8e1dca75fbce980fb5227d6b534bed2ba4f9e3b9")

set(dtk-deps_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -Ddtk_UI_LIB=${toucan_dtk_UI_LIB}
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
