set(HEADERS
    ImageCompTest.h
    ImageReadTest.h)

set(SOURCE
    ImageCompTest.cpp
    ImageReadTest.cpp
    main.cpp)

add_executable(toucan-test ${SOURCE} ${HEADERS})
target_link_libraries(toucan-test toucan)
set_target_properties(toucan-test PROPERTIES FOLDER tests)

add_test(
    toucan-test
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/toucan-test${CMAKE_EXECUTABLE_SUFFIX} ${PROJECT_SOURCE_DIR}/data)
