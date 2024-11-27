set(HEADERS
    App.h
    ClipItem.h
    DocumentTab.h
    Document.h
    DocumentsModel.h
    ExportTool.h
    GapItem.h
    GraphTool.h
    IItem.h
    IToolWidget.h
    InfoBar.h
    JSONTool.h
    MainWindow.h
    MenuBar.h
    PlaybackBar.h
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
    WindowModel.h)

set(SOURCE
    App.cpp
    ClipItem.cpp
    DocumentTab.cpp
    Document.cpp
    DocumentsModel.cpp
    ExportTool.cpp
    GapItem.cpp
    GraphTool.cpp
    IItem.cpp
    IToolWidget.cpp
    InfoBar.cpp
    JSONTool.cpp
    MainWindow.cpp
    MenuBar.cpp
    PlaybackBar.cpp
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
    WindowModel.cpp)

add_library(toucanView ${HEADERS} ${SOURCE})
target_link_libraries(toucanView PUBLIC toucan dtk::dtkUI)
set_target_properties(toucanView PROPERTIES FOLDER lib)
add_dependencies(toucanView ${TOUCAN_PLUGINS})
