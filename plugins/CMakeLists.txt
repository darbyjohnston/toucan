foreach(PLUGIN ColorSpace Draw Filter Generator Transform Transition)
    add_library(
        toucan${PLUGIN} MODULE
        ${PLUGIN}Plugin.h Plugin.h Util.h
        ${PLUGIN}Plugin.cpp Plugin.cpp Util.cpp)
    target_link_libraries(toucan${PLUGIN} PUBLIC OpenImageIO::OpenImageIO MINIZIP::minizip)
    set_target_properties(
        toucan${PLUGIN} PROPERTIES
        FOLDER plugin
        SUFFIX .ofx)

    install(TARGETS toucan${PLUGIN}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION bin
        RUNTIME DESTINATION bin)
endforeach()