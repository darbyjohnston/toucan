include(ExternalProject)

set(yaml-cpp_GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git")
set(yaml-cpp_GIT_TAG "0.8.0")

set(yaml-cpp_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -DYAML_CPP_BUILD_CONTRIB=OFF
    -DYAML_CPP_BUILD_TOOLS=OFF
    -DYAML_CPP_BUILD_TESTS=OFF
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5)

ExternalProject_Add(
    yaml-cpp
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp
    GIT_REPOSITORY ${yaml-cpp_GIT_REPOSITORY}
    GIT_TAG ${yaml-cpp_GIT_TAG}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp-patch/src/emitterutils.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp/src/yaml-cpp/src/emitterutils.cpp
    LIST_SEPARATOR |
    CMAKE_ARGS ${yaml-cpp_ARGS})
