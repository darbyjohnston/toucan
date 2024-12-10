set(HEADERS
    App.h)
set(SOURCE
    App.cpp
    main.cpp)

add_executable(toucan-filmstrip ${HEADERS} ${SOURCE})
target_link_libraries(toucan-filmstrip toucanRender)
set_target_properties(toucan-filmstrip PROPERTIES FOLDER bin)
add_dependencies(toucan-filmstrip ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-filmstrip
    RUNTIME DESTINATION bin)

foreach(OTIO CompositeTracks Draw Filter Gap Generator LinearTimeWarp Transition Transition2 Transform)
    add_test(
        toucan-filmstrip-${OTIO}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/toucan-filmstrip${CMAKE_EXECUTABLE_SUFFIX}
        ${PROJECT_SOURCE_DIR}/data/${OTIO}.otio ${OTIO}.png)
endforeach()
