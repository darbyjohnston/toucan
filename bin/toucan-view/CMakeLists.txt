set(HEADERS
    App.h
    BottomBar.h
    FilesModel.h
    GraphWidget.h
    MenuBar.h
    PlaybackModel.h
    StatusBar.h
    TimeUnitsModel.h
    TimeWidgets.h
    TimelineWidget.h
    ToolBar.h
    ViewModel.h
    Viewport.h
    Window.h)

set(SOURCE
    App.cpp
    BottomBar.cpp
    FilesModel.cpp
    GraphWidget.cpp
    MenuBar.cpp
    PlaybackModel.cpp
    StatusBar.cpp
    TimeUnitsModel.cpp
    TimeWidgets.cpp
    TimelineWidget.cpp
    ToolBar.cpp
    ViewModel.cpp
    Viewport.cpp
    Window.cpp
    main.cpp)

add_executable(toucan-view ${HEADERS} ${SOURCE})
target_link_libraries(toucan-view toucan dtk::dtkUI)
set_target_properties(toucan-view PROPERTIES FOLDER bin)
add_dependencies(toucan-view ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-view
    RUNTIME DESTINATION bin)
