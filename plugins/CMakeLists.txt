set(HEADERS
    InvertPlugin.h)
set(SOURCE
    InvertPlugin.cpp)

add_library(toucan-invert MODULE ${HEADERS} ${SOURCE})
target_link_libraries(toucan-invert PUBLIC OpenImageIO::OpenImageIO)
set_target_properties(toucan-invert PROPERTIES FOLDER plugin)

install(TARGETS toucan-invert
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
