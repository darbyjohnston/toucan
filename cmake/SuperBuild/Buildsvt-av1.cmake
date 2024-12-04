include(ExternalProject)

set(svt-av1_GIT_REPOSITORY "https://gitlab.com/AOMediaCodec/SVT-AV1.git")
set(svt-av1_GIT_TAG "v2.3.0")

set(svt-av1_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -DBUILD_APPS=OFF
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON)

ExternalProject_Add(
    svt-av1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/svt-av1
    GIT_REPOSITORY ${svt-av1_GIT_REPOSITORY}
    GIT_TAG ${svt-av1_GIT_TAG}
    CMAKE_ARGS ${svt-av1_ARGS})
