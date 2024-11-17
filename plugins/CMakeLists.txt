foreach(PLUGIN ColorSpace Draw Filter Generator Transform Transition)
    add_library(
        toucan${PLUGIN} MODULE
        ${PLUGIN}Plugin.h Plugin.h Util.h
        ${PLUGIN}Plugin.cpp Plugin.cpp Util.cpp)
    set(LIBS_PUBLIC OpenImageIO::OpenImageIO MINIZIP::minizip)
    if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        list(APPEND LIBS_PUBLIC stdc++fs)
    endif()
    target_link_libraries(toucan${PLUGIN} PUBLIC ${LIBS_PUBLIC})
    set_target_properties(
        toucan${PLUGIN} PROPERTIES
        FOLDER plugin
        SUFFIX .ofx)

    install(TARGETS toucan${PLUGIN}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION bin
        RUNTIME DESTINATION bin)
endforeach()
