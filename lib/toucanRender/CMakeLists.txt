set(HEADERS
    Comp.h
    FFmpeg.h
    FFmpegRead.h
    FFmpegWrite.h
    ImageEffect.h
    ImageEffectHost.h
    ImageGraph.h
    ImageNode.h
    OTIOZ.h
    Plugin.h
    PropertySet.h
    Read.h
    TimeWarp.h
    TimelineAlgo.h
    TimelineWrapper.h)
set(HEADERS_PRIVATE
    ImageEffect_p.h)
set(SOURCE
    Comp.cpp
    FFmpeg.cpp
    FFmpegRead.cpp
    FFmpegWrite.cpp
    ImageEffect.cpp
    ImageEffectHost.cpp
    ImageGraph.cpp
    ImageNode.cpp
    OTIOZ.cpp
    Plugin.cpp
    PropertySet.cpp
    Read.cpp
    TimeWarp.cpp
    TimelineAlgo.cpp
    TimelineWrapper.cpp)
if(WIN32)
    list(APPEND SOURCE
        PluginWin32.cpp)
else()
    list(APPEND SOURCE
        PluginUnix.cpp)
endif()

add_library(toucanRender ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
target_link_libraries(toucanRender PUBLIC toucanUtil)
set_target_properties(toucanRender PROPERTIES FOLDER lib)
set_target_properties(toucanRender PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(
    TARGETS toucanRender
    EXPORT toucanRenderTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/toucanRender)
install(
    EXPORT toucanRenderTargets
    FILE toucanRenderTargets.cmake
    DESTINATION "lib/cmake/toucan"
    NAMESPACE toucan::)
