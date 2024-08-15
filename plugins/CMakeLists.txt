set(HEADERS
    InvertPlugin.h)
set(SOURCE
    InvertPlugin.cpp)

add_library(InvertPlugin MODULE ${HEADERS} ${SOURCE})
target_link_libraries(InvertPlugin PUBLIC OpenImageIO::OpenImageIO)
set_target_properties(InvertPlugin PROPERTIES FOLDER plugin)

install(TARGETS InvertPlugin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
