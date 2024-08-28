include(ExternalProject)

set(OpenImageIO_GIT_REPOSITORY "https://github.com/AcademySoftwareFoundation/OpenImageIO.git")
# Commit : admin: Relicense code under Apache 2.0 (#3905)
set(OpenImageIO_GIT_TAG "64f829febd352686538beaba10e4ca716a9403a1")

set(OpenImageIO_GIT_REPOSITORY https://github.com/darbyjohnston/OpenImageIO.git)
set(OpenImageIO_GIT_TAG ffmpeg_add_metadata)

set(OpenImageIO_DEPS TIFF PNG libjpeg-turbo OpenEXR OpenColorIO Freetype)
if(toucan_FFMPEG)
    list(APPEND OpenImageIO_DEPS FFmpeg)
endif()

set(OpenImageIO_ARGS
    ${toucan_EXTERNAL_PROJECT_ARGS}
    -DOIIO_BUILD_TESTS=OFF
    -DOIIO_BUILD_TOOLS=OFF
    -DOIIO_BUILD_DOCS=OFF
    -DOIIO_INSTALL_DOCS=OFF
    -DOIIO_INSTALL_FONTS=ON
    -DUSE_FREETYPE=ON
    -DUSE_PNG=ON
    -DUSE_FFMPEG=${toucan_FFMPEG}
    -DUSE_OPENCOLORIO=ON
    -DUSE_BZIP2=OFF
    -DUSE_DCMTK=OFF
    -DUSE_GIF=OFF
    -DUSE_JXL=OFF
    -DUSE_LIBHEIF=OFF
    -DUSE_LIBRAW=OFF
    -DUSE_NUKE=OFF
    -DUSE_OPENCV=OFF
    -DUSE_OPENJPEG=OFF
    -DUSE_OPENVDB=OFF
    -DUSE_PTEX=OFF
    -DUSE_PYTHON=OFF
    -DUSE_QT=OFF
    -DUSE_TBB=OFF
    -DUSE_WEBP=OFF)

ExternalProject_Add(
    OpenImageIO
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenImageIO
    DEPENDS ${OpenImageIO_DEPS}
    GIT_REPOSITORY ${OpenImageIO_GIT_REPOSITORY}
    GIT_TAG ${OpenImageIO_GIT_TAG}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO-patch/ffmpeginput.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/OpenImageIO/src/OpenImageIO/src/ffmpeg.imageio/ffmpeginput.cpp
    CMAKE_ARGS ${OpenImageIO_ARGS})
