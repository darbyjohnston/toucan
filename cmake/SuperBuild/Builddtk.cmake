include(ExternalProject)

set(dtk_GIT_REPOSITORY "https://github.com/darbyjohnston/dtk.git")
set(dtk_GIT_TAG "c16c9bd079c51d36b431e18108e39682f7c4c37c")

set(dtk_DEPS dtk-deps)
set(dtk_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -Ddtk_UI_LIB=${toucan_dtk_UI_LIB}
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
