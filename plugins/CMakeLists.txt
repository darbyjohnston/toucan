foreach(PLUGIN Draw Filter Generator Transform)
    string(TOLOWER ${PLUGIN} PLUGIN_LOWER)
    add_library(
        toucan-${PLUGIN_LOWER} MODULE
        ${PLUGIN}Plugin.h Plugin.h Util.h
        ${PLUGIN}Plugin.cpp Plugin.cpp Util.cpp)
    target_link_libraries(toucan-${PLUGIN_LOWER} PUBLIC OpenImageIO::OpenImageIO)
    set_target_properties(toucan-${PLUGIN_LOWER} PROPERTIES FOLDER plugin)

    install(TARGETS toucan-${PLUGIN_LOWER}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
endforeach()