add_executable(toucan-edit main.cpp)
target_link_libraries(toucan-edit toucanEdit)
set_target_properties(toucan-edit PROPERTIES FOLDER bin)
add_dependencies(toucan-edit ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-edit
    RUNTIME DESTINATION bin)
