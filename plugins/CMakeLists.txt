add_library(toucanPlugin Plugin.h Util.h Plugin.cpp Util.cpp)
set(LIBS_PUBLIC OpenImageIO::OpenImageIO MINIZIP::minizip)
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    list(APPEND LIBS_PUBLIC stdc++fs)
endif()
target_link_libraries(toucanPlugin PUBLIC ${LIBS_PUBLIC})
set_target_properties(toucanPlugin PROPERTIES FOLDER plugin)
set_target_properties(toucanPlugin PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(
    TARGETS toucanPlugin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/toucan)

foreach(PLUGIN Color Draw Filter Generator Transform Transition)
    set(PLUGIN_NAME toucan${PLUGIN}Plugin)
    add_library(
        ${PLUGIN_NAME} MODULE
        ${PLUGIN}Plugin.h
        ${PLUGIN}Plugin.cpp )
    target_link_libraries(${PLUGIN_NAME} PUBLIC toucanPlugin)
    set_target_properties(
        ${PLUGIN_NAME} PROPERTIES
        FOLDER plugin
        SUFFIX .ofx)

    install(TARGETS ${PLUGIN_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION bin
        RUNTIME DESTINATION bin)
endforeach()
