set(HEADERS
    App.h
    BottomBar.h
    Clip.h
    ClipWidget.h
    Document.h
    DocumentsModel.h
    DocumentTab.h
    Gap.h
    GapWidget.h
    IContainer.h
    IItem.h
    IItemWidget.h
    ImageGraph.h
    MenuBar.h
    PlaybackModel.h
    SelectionModel.h
    Stack.h
    StackWidget.h
    ThumbnailGenerator.h
    Timeline.h
    TimelineView.h
    TimelineWidget.h
    TimeUnitsModel.h
    TimeWidgets.h
    Track.h
    TrackWidget.h
    Viewport.h
    ViewModel.h
    Window.h)

set(SOURCE
    App.cpp
    BottomBar.cpp
    Clip.cpp
    ClipWidget.cpp
    Document.cpp
    DocumentsModel.cpp
    DocumentTab.cpp
    Gap.cpp
    GapWidget.cpp
    IContainer.cpp
    IItem.cpp
    IItemWidget.cpp
    ImageGraph.cpp
    MenuBar.cpp
    PlaybackModel.cpp
    SelectionModel.cpp
    Stack.cpp
    StackWidget.cpp
    ThumbnailGenerator.cpp
    Timeline.cpp
    TimelineView.cpp
    TimelineWidget.cpp
    TimeUnitsModel.cpp
    TimeWidgets.cpp
    Track.cpp
    TrackWidget.cpp
    Viewport.cpp
    ViewModel.cpp
    Window.cpp
    main.cpp)

add_executable(toucan-edit ${HEADERS} ${SOURCE})
target_link_libraries(toucan-edit toucan dtk::dtkUI)
set_target_properties(toucan-edit PROPERTIES FOLDER bin)
add_dependencies(toucan-edit ${TOUCAN_PLUGINS})

install(
    TARGETS toucan-edit
    RUNTIME DESTINATION bin)
