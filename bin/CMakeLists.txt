add_executable(toucan-render main.cpp)
target_link_libraries(toucan-render toucan)
set_target_properties(toucan-render PROPERTIES FOLDER bin)

install(
    TARGETS toucan-render
    RUNTIME DESTINATION bin)
