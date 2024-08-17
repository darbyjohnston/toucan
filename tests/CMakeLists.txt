set(HEADERS
    CompTest.h
    PropertySetTest.h
    ReadTest.h
    TimelineGraphTest.h
    UtilTest.h)

set(SOURCE
    CompTest.cpp
    PropertySetTest.cpp
    ReadTest.cpp
    TimelineGraphTest.cpp
    UtilTest.cpp
    main.cpp)

add_executable(toucan-test ${SOURCE} ${HEADERS})
target_link_libraries(toucan-test toucan)
set_target_properties(toucan-test PROPERTIES FOLDER tests)
add_dependencies(toucan-test ${TOUCAN_PLUGINS})

add_test(
    toucan-test
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/toucan-test${CMAKE_EXECUTABLE_SUFFIX} ${PROJECT_SOURCE_DIR}/data)
