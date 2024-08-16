set(HEADERS
    BoxOp.h
    CheckersOp.h
    ColorMapOp.h
    CompOp.h
    FillOp.h
    FlipOp.h
    FlopOp.h
    Host.h
    ImageOp.h
    Init.h
    InvertOp.h
    LineOp.h
    LinearTimeWarpOp.h
    NoiseOp.h
    Plugin.h
    PowOp.h
    PropertySet.h
    ReadOp.h
    ResizeOp.h
    RotateOp.h
    SaturateOp.h
    SequenceReadOp.h
    TextOp.h
    TimelineTraverse.h
    TransitionOp.h
    Util.h)
set(HEADERS_PRIVATE)
set(SOURCE
    BoxOp.cpp
    CheckersOp.cpp
    ColorMapOp.cpp
    CompOp.cpp
    FillOp.cpp
    FlipOp.cpp
    FlopOp.cpp
    Host.cpp
    ImageOp.cpp
    Init.cpp
    InvertOp.cpp
    LineOp.cpp
    LinearTimeWarpOp.cpp
    NoiseOp.cpp
    Plugin.cpp
    PowOp.cpp
    PropertySet.cpp
    ReadOp.cpp
    ResizeOp.cpp
    RotateOp.cpp
    SaturateOp.cpp
    SequenceReadOp.cpp
    TextOp.cpp
    TimelineTraverse.cpp
    TransitionOp.cpp
    Util.cpp)
if(WIN32)
    list(APPEND SOURCE UtilWin32.cpp)
endif()

add_library(toucan ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
target_link_libraries(toucan PUBLIC OTIO OpenImageIO::OpenImageIO)
set_target_properties(toucan PROPERTIES FOLDER lib)
set_target_properties(toucan PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS toucan
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include)
