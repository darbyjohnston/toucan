set(HEADERS
    ImageComp.h
    ImageRead.h
    ImageOp.h)
set(SOURCE
    ImageComp.cpp
    ImageRead.cpp
    ImageOp.cpp)

add_library(toucan ${HEADERS} ${SOURCE})
target_link_libraries(toucan PUBLIC OTIO OpenImageIO::OpenImageIO)
set_target_properties(toucan PROPERTIES FOLDER lib)
set_target_properties(toucan PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS toucan
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include)