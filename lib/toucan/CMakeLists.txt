set(HEADERS
    Box.h
    Checkers.h
    ColorMap.h
    Comp.h
    Fill.h
    Flip.h
    Flop.h
    ImageNode.h
    ImageEffectHost.h
    Init.h
    Invert.h
    Line.h
    LinearTimeWarp.h
    Noise.h
    Plugin.h
    Pow.h
    PropertySet.h
    Read.h
    Resize.h
    Rotate.h
    Saturate.h
    SequenceRead.h
    Text.h
    TimelineGraph.h
    Transition.h
    Util.h)
set(HEADERS_PRIVATE)
set(SOURCE
    Box.cpp
    Checkers.cpp
    ColorMap.cpp
    Comp.cpp
    Fill.cpp
    Flip.cpp
    Flop.cpp
    ImageNode.cpp
    ImageEffectHost.cpp
    Init.cpp
    Invert.cpp
    Line.cpp
    LinearTimeWarp.cpp
    Noise.cpp
    Plugin.cpp
    Pow.cpp
    PropertySet.cpp
    Read.cpp
    Resize.cpp
    Rotate.cpp
    Saturate.cpp
    SequenceRead.cpp
    Text.cpp
    TimelineGraph.cpp
    Transition.cpp
    Util.cpp)
if(WIN32)
    list(APPEND SOURCE
        PluginWin32.cpp
        UtilWin32.cpp)
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
