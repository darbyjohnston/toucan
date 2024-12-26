set(HEADERS
    FilesModelTest.h
    PlaybackModelTest.h
    SelectionModelTest.h
    ViewModelTest.h
    WindowModelTest.h)

set(SOURCE
    FilesModelTest.cpp
    PlaybackModelTest.cpp
    SelectionModelTest.cpp
    ViewModelTest.cpp
    WindowModelTest.cpp)

add_library(toucanViewTest ${SOURCE} ${HEADERS})
target_link_libraries(toucanViewTest toucanView)
set_target_properties(toucanViewTest PROPERTIES FOLDER tests)
