set(LIBS toucanTest)
if(toucan_EDIT)
    list(APPEND LIBS toucanEditTest)
endif()

add_executable(toucan-test main.cpp)
target_link_libraries(toucan-test ${LIBS})
set_target_properties(toucan-test PROPERTIES FOLDER tests)
add_dependencies(toucan-test ${TOUCAN_PLUGINS})

add_test(
    toucan-test
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/toucan-test${CMAKE_EXECUTABLE_SUFFIX} ${PROJECT_SOURCE_DIR}/data)
