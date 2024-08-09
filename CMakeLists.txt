cmake_minimum_required(VERSION 3.28)

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
endif()

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(TIFF REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(OpenImageIO REQUIRED)
find_package(OTIO REQUIRED)

include_directories(lib)

add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(tests)
