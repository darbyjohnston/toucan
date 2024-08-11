set(HEADERS
    CheckersOp.h
    CompOp.h
    FillOp.h
    FlipOp.h
    FlopOp.h
    ImageOp.h
    Init.h
    InvertOp.h
    NoiseOp.h
    ReadOp.h
    ResizeOp.h
    RotateOp.h
    SaturateOp.h
    SequenceReadOp.h
    TextOp.h
    TimelineTraverse.h)
set(SOURCE
    CheckersOp.cpp
    CompOp.cpp
    FillOp.cpp
    FlipOp.cpp
    FlopOp.cpp
    ImageOp.cpp
    Init.cpp
    InvertOp.cpp
    NoiseOp.cpp
    ReadOp.cpp
    ResizeOp.cpp
    RotateOp.cpp
    SaturateOp.cpp
    SequenceReadOp.cpp
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