foreach(PLUGIN ColorSpace Draw Filter Generator Transform Transition)
    add_library(
        toucan${PLUGIN} MODULE
        ${PLUGIN}Plugin.h Plugin.h Util.h
        ${PLUGIN}Plugin.cpp Plugin.cpp Util.cpp)
    set(LIBS OpenImageIO::OpenImageIO)
    if(TARGET MINIZIP::minizip)
        list(APPEND LIBS MINIZIP::minizip)
    endif()
    target_link_libraries(toucan${PLUGIN} PUBLIC ${LIBS})
    set_target_properties(
        toucan${PLUGIN} PROPERTIES
        FOLDER plugin
        SUFFIX .ofx)

    install(TARGETS toucan${PLUGIN}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION bin
        RUNTIME DESTINATION bin)
endforeach()
