set(HEADERS
    AudioBuffer.h
    AudioGraph.h
    Comp.h
    FFmpeg.h
    FFmpegAudioRead.h
    FFmpegAudioWrite.h
    FFmpegRead.h
    FFmpegWrite.h
    ImageEffect.h
    ImageEffectHost.h
    ImageGraph.h
    ImageNode.h
    MemoryMap.h
    Plugin.h
    PropertySet.h
    Read.h
    TimeWarp.h
    TimelineAlgo.h
    TimelineWrapper.h
    Util.h)
set(HEADERS_PRIVATE)
set(SOURCE
    AudioGraph.cpp
    Comp.cpp
    FFmpeg.cpp
    FFmpegAudioRead.cpp
    FFmpegAudioWrite.cpp
    FFmpegRead.cpp
    FFmpegWrite.cpp
    ImageEffect.cpp
    ImageEffectHost.cpp
    ImageGraph.cpp
    ImageNode.cpp
    MemoryMap.cpp
    Plugin.cpp
    PropertySet.cpp
    Read.cpp
    TimeWarp.cpp
    TimelineAlgo.cpp
    TimelineWrapper.cpp
    Util.cpp)
if(WIN32)
    list(APPEND SOURCE
        MemoryMapWin32.cpp
        PluginWin32.cpp)
else()
    list(APPEND SOURCE
        MemoryMapUnix.cpp
        PluginUnix.cpp)
endif()

add_library(toucanRender ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
find_library(SWRESAMPLE_LIBRARY swresample PATHS ${CMAKE_INSTALL_PREFIX}/lib NO_DEFAULT_PATH)
if(NOT SWRESAMPLE_LIBRARY)
    find_library(SWRESAMPLE_LIBRARY swresample)
endif()
set(LIBS_PUBLIC
    toucanResource
    ftk::ftkCore
    OTIO::opentimelineio
    OTIO::opentime
    lunasvg::lunasvg
    OpenImageIO::OpenImageIO
    MINIZIP::minizip-ng)
if(SWRESAMPLE_LIBRARY)
    list(APPEND LIBS_PUBLIC ${SWRESAMPLE_LIBRARY})
endif()
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    list(APPEND LIBS_PUBLIC stdc++fs)
endif()
target_link_libraries(toucanRender PUBLIC ${LIBS_PUBLIC})
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
