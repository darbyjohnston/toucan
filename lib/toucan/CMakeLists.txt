set(HEADERS
    CheckersOp.h
    CompOp.h
    FillOp.h
    ImageOp.h
    Init.h
    NoiseOp.h
    ReadOp.h
    ResizeOp.h
    RotateOp.h
    TextOp.h
    TimelineTraverse.h)
set(SOURCE
    CheckersOp.cpp
    CompOp.cpp
    FillOp.cpp
    ImageOp.cpp
    Init.cpp
    NoiseOp.cpp
    ReadOp.cpp
    ResizeOp.cpp
    RotateOp.cpp
    TextOp.cpp
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