set(HEADERS
    StackTest.h)

set(SOURCE
    StackTest.cpp)

add_library(toucanEditTest ${SOURCE} ${HEADERS})
target_link_libraries(toucanEditTest toucanEdit)
set_target_properties(toucanEditTest PROPERTIES FOLDER tests)
