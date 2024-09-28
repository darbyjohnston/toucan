set(HEADERS
    CompTest.h
    ImageGraphTest.h
    PropertySetTest.h
    ReadTest.h
    UtilTest.h)

set(SOURCE
    CompTest.cpp
    ImageGraphTest.cpp
    PropertySetTest.cpp
    ReadTest.cpp
    UtilTest.cpp)

add_library(toucanTest ${SOURCE} ${HEADERS})
target_link_libraries(toucanTest toucan)
set_target_properties(toucanTest PROPERTIES FOLDER tests)
