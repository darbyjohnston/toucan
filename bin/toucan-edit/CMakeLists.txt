set(HEADERS
    App.h
    Clip.h
    IContainer.h
    IItem.h
    Stack.h
    Timeline.h
    Track.h
    Window.h)

set(SOURCE
    App.cpp
    Clip.cpp
    IContainer.cpp
    IItem.cpp
    Stack.cpp
    Timeline.cpp
    Track.cpp
    Window.cpp
    main.cpp)

add_executable(toucan-edit ${HEADERS} ${SOURCE})
target_link_libraries(toucan-edit toucan dtk::dtkUI)
set_target_properties(toucan-edit PROPERTIES FOLDER bin)
add_dependencies(toucan-edit ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-edit
    RUNTIME DESTINATION bin)
