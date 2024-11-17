set(HEADERS
    Comp.h
    FFmpegRead.h
    ImageEffect.h
    ImageEffectHost.h
    ImageGraph.h
    ImageNode.h
    MemoryMap.h
    MessageLog.h
    Plugin.h
    PropertySet.h
    Read.h
    TimeWarp.h
    TimelineWrapper.h
    Util.h)
set(HEADERS_PRIVATE
    ImageEffect_p.h)
set(SOURCE
    Comp.cpp
    FFmpegRead.cpp
    ImageEffect.cpp
    ImageEffectHost.cpp
    ImageGraph.cpp
    ImageNode.cpp
    MemoryMap.cpp
    MessageLog.cpp
    Plugin.cpp
    PropertySet.cpp
    Read.cpp
    TimeWarp.cpp
    TimelineWrapper.cpp
    Util.cpp)
if(WIN32)
    list(APPEND SOURCE
        MemoryMapWin32.cpp
        PluginWin32.cpp
        UtilWin32.cpp)
else()
    list(APPEND SOURCE
        MemoryMapUnix.cpp
        PluginUnix.cpp
        UtilUnix.cpp)
endif()

add_library(toucan ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
set(LIBS_PUBLIC OTIO::opentimelineio OTIO::opentime OpenImageIO::OpenImageIO MINIZIP::minizip)
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    list(APPEND LIBS_PUBLIC stdc++fs)
endif()
target_link_libraries(toucan PUBLIC ${LIBS_PUBLIC})
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
