set(HEADERS
    FileTest.h)

set(SOURCE
    FileTest.cpp)

add_library(toucanUtilTest ${SOURCE} ${HEADERS})
target_link_libraries(toucanUtilTest toucanUtil)
set_target_properties(toucanUtilTest PROPERTIES FOLDER tests)
