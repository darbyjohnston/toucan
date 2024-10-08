cmake_minimum_required(VERSION 3.22)

project(
    toucan
    VERSION 0.1.0
    DESCRIPTION "toucan"
    HOMEPAGE_URL "https://github.com/darbyjohnston/toucan"
    LANGUAGES CXX C)

set(toucan_VIEW ON CACHE BOOL "Build viewer application")
set(toucan_EDIT OFF CACHE BOOL "Build editor application")

list(PREPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
if(NOT BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF)
endif()
if(NOT CMAKE_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)
endif()
if(APPLE)
    set(CMAKE_FIND_FRAMEWORK LAST)
    set(CMAKE_IGNORE_PREFIX_PATH /opt/homebrew)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if(toucan_EDIT)
    add_definitions(-Dtoucan_EDIT)
endif()

set(TOUCAN_PLUGINS
    toucanColorSpace
    toucanDraw
    toucanFilter
    toucanGenerator
    toucanTransform
    toucanTransition)

enable_testing()

find_package(Imath)
find_package(Freetype)
find_package(ZLIB)
find_package(PNG)
find_package(JPEG)
find_package(TIFF)
find_package(OpenEXR)
find_package(minizip)
find_package(OpenColorIO)
find_package(OpenImageIO)
find_package(OTIO)
find_package(OpenFX)
if(toucan_VIEW OR toucan_EDIT)
    find_package(dtk)
endif()

include_directories(lib)
include_directories(tests)
# \todo OpenTimelineIO doesn't use the "Imath" prefix for includes?
include_directories(${CMAKE_INSTALL_PREFIX}/include/Imath)

add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(bin)
add_subdirectory(tests)

install(
    FILES cmake/FindOpenFX.cmake
    DESTINATION lib/cmake/toucan)
install(
    FILES cmake/FindOTIO.cmake
    DESTINATION lib/cmake/toucan)
include(CMakePackageConfigHelpers)
set(INCLUDE_INSTALL_DIR include/toucan)
configure_package_config_file(
    toucanConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/toucanConfig.cmake
    INSTALL_DESTINATION lib/cmake/toucan
    PATH_VARS INCLUDE_INSTALL_DIR)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/toucanConfig.cmake
    DESTINATION lib/cmake/toucan)
