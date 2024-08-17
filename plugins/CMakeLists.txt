set(HEADERS
    FilterPlugin.h
    Plugin.h
    Util.h)
set(SOURCE
    FilterPlugin.cpp
    Plugin.cpp
    Util.cpp)

add_library(toucan-filter MODULE ${HEADERS} ${SOURCE})
target_link_libraries(toucan-filter PUBLIC OpenImageIO::OpenImageIO)
set_target_properties(toucan-filter PROPERTIES FOLDER plugin)

install(TARGETS toucan-filter
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
