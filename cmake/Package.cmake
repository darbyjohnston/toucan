 set(CPACK_PACKAGE_FILE_NAME
    ${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

if(WIN32)
    set(CPACK_GENERATOR ZIP)
    
    set(INSTALL_DLLS
        ${CMAKE_INSTALL_PREFIX}/bin/avcodec-61.dll
        ${CMAKE_INSTALL_PREFIX}/bin/avdevice-61.dll
        ${CMAKE_INSTALL_PREFIX}/bin/avformat-61.dll
        ${CMAKE_INSTALL_PREFIX}/bin/avutil-59.dll
        ${CMAKE_INSTALL_PREFIX}/bin/swresample-5.dll
        ${CMAKE_INSTALL_PREFIX}/bin/swscale-8.dll)
    
    install(
        FILES ${INSTALL_DLLS}
        DESTINATION bin)

elseif(APPLE)

    set(CPACK_GENERATOR TGZ)

    list(APPEND CMAKE_INSTALL_RPATH
        "@executable_path/../lib")
    #set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    set(INSTALL_DYLIBS
        ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.61.19.100.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.61.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.61.3.100.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.61.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavformat.61.7.100.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavformat.61.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavformat.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavutil.59.39.100.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavutil.59.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libavutil.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libswresample.5.3.100.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libswresample.5.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libswresample.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libswscale.8.3.100.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libswscale.8.dylib
        ${CMAKE_INSTALL_PREFIX}/lib/libswscale.dylib)
        
    install(
        FILES ${INSTALL_DYLIBS}
        DESTINATION lib)

else()

    set(CPACK_GENERATOR TGZ)

    set(INSTALL_LIBS
        ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.so
        ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.so.61
        ${CMAKE_INSTALL_PREFIX}/lib/libavcodec.so.61.19.100
        ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.so
        ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.so.61
        ${CMAKE_INSTALL_PREFIX}/lib/libavdevice.so.61.3.100
        ${CMAKE_INSTALL_PREFIX}/lib/libavformat.so
        ${CMAKE_INSTALL_PREFIX}/lib/libavformat.so.61
        ${CMAKE_INSTALL_PREFIX}/lib/libavformat.so.61.7.100
        ${CMAKE_INSTALL_PREFIX}/lib/libavutil.so
        ${CMAKE_INSTALL_PREFIX}/lib/libavutil.so.59
        ${CMAKE_INSTALL_PREFIX}/lib/libavutil.so.59.39.100
        ${CMAKE_INSTALL_PREFIX}/lib/libswresample.so
        ${CMAKE_INSTALL_PREFIX}/lib/libswresample.so.5
        ${CMAKE_INSTALL_PREFIX}/lib/libswresample.so.5.3.100
        ${CMAKE_INSTALL_PREFIX}/lib/libswscale.so
        ${CMAKE_INSTALL_PREFIX}/lib/libswscale.so.8
        ${CMAKE_INSTALL_PREFIX}/lib/libswscale.so.8.3.100)
    
    install(
        FILES ${INSTALL_LIBS}
        DESTINATION lib)

endif()
