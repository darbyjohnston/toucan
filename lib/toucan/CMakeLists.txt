set(HEADERS
    ColorSpace.h
    Comp.h
    Draw.h
    Filter.h
    ImageEffect.h
    ImageEffectHost.h
    ImageGraph.h
    ImageNode.h
    Init.h
    MessageLog.h
    Plugin.h
    PropertySet.h
    Read.h
    TimeWarp.h
    Transform.h
    Util.h)
set(HEADERS_PRIVATE)
set(SOURCE
    ColorSpace.cpp
    Comp.cpp
    Draw.cpp
    Filter.cpp
    ImageEffect.cpp
    ImageEffectHost.cpp
    ImageGraph.cpp
    ImageNode.cpp
    Init.cpp
    MessageLog.cpp
    Plugin.cpp
    PropertySet.cpp
    Read.cpp
    TimeWarp.cpp
    Transform.cpp
    Util.cpp)
if(WIN32)
    list(APPEND SOURCE
        PluginWin32.cpp
        UtilWin32.cpp)
else()
    list(APPEND SOURCE
        PluginUnix.cpp)
endif()

add_library(toucan ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
target_link_libraries(toucan PUBLIC OTIO::opentimelineio OTIO::opentime OpenImageIO::OpenImageIO MINIZIP::minizip)
set_target_properties(toucan PROPERTIES FOLDER lib)
set_target_properties(toucan PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(
    TARGETS toucan
    EXPORT toucanTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/toucan)
install(
    EXPORT toucanTargets
    FILE toucanTargets.cmake
    DESTINATION "lib/cmake/toucan"
    NAMESPACE toucan::)
