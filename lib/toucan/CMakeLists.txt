set(HEADERS
    CheckersImageOp.h
    CompImageOp.h
    FillImageOp.h
    ImageOp.h
    Init.h
    NoiseImageOp.h
    ReadImageOp.h
    TimelineTraverse.h)
set(SOURCE
    CheckersImageOp.cpp
    CompImageOp.cpp
    FillImageOp.cpp
    ImageOp.cpp
    Init.cpp
    NoiseImageOp.cpp
    ReadImageOp.cpp
    TimelineTraverse.cpp)

add_library(toucan ${HEADERS} ${SOURCE})
target_link_libraries(toucan PUBLIC OTIO OpenImageIO::OpenImageIO)
set_target_properties(toucan PROPERTIES FOLDER lib)
set_target_properties(toucan PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS toucan
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include)