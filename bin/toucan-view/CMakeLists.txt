add_executable(toucan-view main.cpp)
target_link_libraries(toucan-view toucanView)
set_target_properties(toucan-view PROPERTIES FOLDER bin)
add_dependencies(toucan-view ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-view
    RUNTIME DESTINATION bin)
