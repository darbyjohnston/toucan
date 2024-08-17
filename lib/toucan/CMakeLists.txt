set(HEADERS
    Comp.h
    Draw.h
    Filter.h
    Generator.h
    TimeWarp.h
    Transform.h
    Transition.h
    ImageNode.h
    ImageEffectHost.h
    Init.h
    Plugin.h
    PropertySet.h
    Read.h
    TimelineGraph.h
    Transition.h
    Util.h)
set(HEADERS_PRIVATE)
set(SOURCE
    Comp.cpp
    Draw.cpp
    Filter.cpp
    Generator.cpp
    TimeWarp.cpp
    Transform.cpp
    Transition.cpp
    ImageNode.cpp
    ImageEffectHost.cpp
    Init.cpp
    Plugin.cpp
    PropertySet.cpp
    Read.cpp
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
