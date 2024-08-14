set(HEADERS
    BoxOp.h
    CheckersOp.h
    ColorMapOp.h
    CompOp.h
    FillOp.h
    FlipOp.h
    FlopOp.h
    ImageOp.h
    Init.h
    InvertOp.h
    LineOp.h
    LinearTimeWarpOp.h
    NoiseOp.h
    PowOp.h
    ReadOp.h
    ResizeOp.h
    RotateOp.h
    SaturateOp.h
    SequenceReadOp.h
    TextOp.h
    TimelineTraverse.h
    TransitionOp.h
    Util.h)
set(SOURCE
    BoxOp.cpp
    CheckersOp.cpp
    ColorMapOp.cpp
    CompOp.cpp
    FillOp.cpp
    FlipOp.cpp
    FlopOp.cpp
    ImageOp.cpp
    Init.cpp
    InvertOp.cpp
    LineOp.cpp
    LinearTimeWarpOp.cpp
    NoiseOp.cpp
    PowOp.cpp
    ReadOp.cpp
    ResizeOp.cpp
    RotateOp.cpp
    SaturateOp.cpp
    SequenceReadOp.cpp
    TextOp.cpp
    TimelineTraverse.cpp
    TransitionOp.cpp
    Util.cpp)

add_library(toucan ${HEADERS} ${SOURCE})
target_link_libraries(toucan PUBLIC OTIO OpenImageIO::OpenImageIO)
set_target_properties(toucan PROPERTIES FOLDER lib)
set_target_properties(toucan PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS toucan
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include)
