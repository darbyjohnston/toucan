set(HEADERS
    App.h
    GraphWidget.h
    MenuBar.h
    TimeWidget.h
    TimelineWidget.h
    Type.h
    Viewport.h
    Window.h)

set(SOURCE
    App.cpp
    GraphWidget.cpp
    MenuBar.cpp
    TimeWidget.cpp
    TimelineWidget.cpp
    Window.cpp
    Viewport.cpp
    main.cpp)

add_executable(toucan-view ${HEADERS} ${SOURCE})
target_link_libraries(toucan-view toucan dtk::dtkUIApp)
set_target_properties(toucan-view PROPERTIES FOLDER bin)
add_dependencies(toucan-view ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-view
    RUNTIME DESTINATION bin)
