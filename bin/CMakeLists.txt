add_executable(toucan-render main.cpp)
target_link_libraries(toucan-render toucan)
set_target_properties(toucan-render PROPERTIES FOLDER bin)
add_dependencies(toucan-render toucan-filter)

install(
    TARGETS toucan-render
    RUNTIME DESTINATION bin)

foreach(OTIO CompositeTracks Filters Gap LinearTimeWarp Patterns Render Transition Transition2 Transforms)
    add_test(
        toucan-render-${OTIO}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/toucan-render${CMAKE_EXECUTABLE_SUFFIX}
        ${PROJECT_SOURCE_DIR}/data/${OTIO}.otio ${OTIO}.png -filmstrip)
endforeach()
