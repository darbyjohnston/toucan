set(HEADERS
    App.h
    BottomBar.h
    ClipItem.h
    DocumentTab.h
    Document.h
    DocumentsModel.h
    Export.h
    ExportTool.h
    GapItem.h
    GraphTool.h
    IItem.h
    IToolWidget.h
    InspectorTool.h
    MenuBar.h
    PlaybackModel.h
    SelectionModel.h
    StackItem.h
    ThumbnailGenerator.h
    TimeUnitsModel.h
    TimeWidgets.h
    TimelineItem.h
    TimelineWidget.h
    ToolBar.h
    TrackItem.h
    ViewModel.h
    Viewport.h
    Window.h
    WindowModel.h)

set(SOURCE
    App.cpp
    BottomBar.cpp
    ClipItem.cpp
    DocumentTab.cpp
    Document.cpp
    DocumentsModel.cpp
    Export.cpp
    ExportTool.cpp
    GapItem.cpp
    GraphTool.cpp
    IItem.cpp
    IToolWidget.cpp
    InspectorTool.cpp
    MenuBar.cpp
    PlaybackModel.cpp
    SelectionModel.cpp
    StackItem.cpp
    ThumbnailGenerator.cpp
    TimeUnitsModel.cpp
    TimeWidgets.cpp
    TimelineItem.cpp
    TimelineWidget.cpp
    ToolBar.cpp
    TrackItem.cpp
    ViewModel.cpp
    Viewport.cpp
    Window.cpp
    WindowModel.cpp
    main.cpp)

add_executable(toucan-view ${HEADERS} ${SOURCE})
target_link_libraries(toucan-view toucan dtk::dtkUI)
set_target_properties(toucan-view PROPERTIES FOLDER bin)
add_dependencies(toucan-view ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-view
    RUNTIME DESTINATION bin)
