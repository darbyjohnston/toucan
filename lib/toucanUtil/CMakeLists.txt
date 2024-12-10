set(HEADERS
    CmdLine.h
    CmdLineInline.h
    File.h
    LRUCache.h
    LRUCacheInline.h
    Math.h
    MemoryMap.h
    MessageLog.h
    String.h)
set(HEADERS_PRIVATE)
set(SOURCE
    CmdLine.cpp
    File.cpp
    Math.cpp
    MemoryMap.cpp
    MessageLog.cpp
    String.cpp)
if(WIN32)
    list(APPEND SOURCE
        FileWin32.cpp
        MemoryMapWin32.cpp)
else()
    list(APPEND SOURCE
        FileUnix.cpp
        MemoryMapUnix.cpp)
endif()

add_library(toucanUtil ${HEADERS} ${HEADERS_PRIVATE} ${SOURCE})
set(LIBS_PUBLIC OTIO::opentimelineio OTIO::opentime OpenImageIO::OpenImageIO MINIZIP::minizip)
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    list(APPEND LIBS_PUBLIC stdc++fs)
endif()
target_link_libraries(toucanUtil PUBLIC ${LIBS_PUBLIC})
set_target_properties(toucanUtil PROPERTIES FOLDER lib)
set_target_properties(toucanUtil PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(
    TARGETS toucanUtil
    EXPORT toucanUtilTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/toucanUtil)
install(
    EXPORT toucanUtilTargets
    FILE toucanUtilTargets.cmake
    DESTINATION "lib/cmake/toucan"
    NAMESPACE toucan::)
