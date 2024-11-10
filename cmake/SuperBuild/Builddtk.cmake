include(ExternalProject)

set(dtk_GIT_REPOSITORY "https://github.com/darbyjohnston/dtk.git")
set(dtk_GIT_TAG "d3fb154f63d96f3f3d8bb6b650c4165049767824")

set(dtk_DEPS dtk-deps)
set(dtk_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -Ddtk_PYTHON=OFF
    -Ddtk_TESTS=OFF
    -Ddtk_EXAMPLES=OFF)

ExternalProject_Add(
    dtk
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/dtk
    DEPENDS ${dtk_DEPS}
    GIT_REPOSITORY ${dtk_GIT_REPOSITORY}
    GIT_TAG ${dtk_GIT_TAG}
    LIST_SEPARATOR |
    CMAKE_ARGS ${dtk_ARGS})
