cmake_minimum_required(VERSION 3.22)

project(
    toucan
    VERSION 0.1.0
    DESCRIPTION "toucan"
    HOMEPAGE_URL "https://github.com/darbyjohnston/toucan"
    LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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

set(TOUCAN_PLUGINS
    toucanColorSpace
    toucanDraw
    toucanFilter
    toucanGenerator
    toucanTransform
    toucanTransition)

enable_testing()

find_package(Imath REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(TIFF REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(minizip REQUIRED)
find_package(OpenColorIO REQUIRED)
find_package(OpenImageIO REQUIRED)
find_package(OTIO REQUIRED)
find_package(OpenFX REQUIRED)

include_directories(lib)
# \todo OpenTimelineIO doesn't use the "Imath" prefix for includes?
include_directories(${CMAKE_INSTALL_PREFIX}/include/Imath)

add_subdirectory(lib/toucan)
add_subdirectory(plugins)
add_subdirectory(bin)
add_subdirectory(tests)
