set(HEADERS
    CompTest.h
    ImageGraphTest.h
    PropertySetTest.h
    ReadTest.h)

set(SOURCE
    CompTest.cpp
    ImageGraphTest.cpp
    PropertySetTest.cpp
    ReadTest.cpp)

add_library(toucanRenderTest ${SOURCE} ${HEADERS})
target_link_libraries(toucanRenderTest toucanRender)
set_target_properties(toucanRenderTest PROPERTIES FOLDER tests)
