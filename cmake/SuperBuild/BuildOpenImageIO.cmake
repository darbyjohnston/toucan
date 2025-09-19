include(ExternalProject)

set(OpenImageIO_GIT_REPOSITORY "https://github.com/AcademySoftwareFoundation/OpenImageIO.git")
set(OpenImageIO_GIT_TAG "v3.0.10.0")

set(OpenImageIO_DEPS)
if(toucan_PNG)
    list(APPEND OpenImageIO_DEPS PNG)
endif()
if(toucan_TIFF)
    list(APPEND OpenImageIO_DEPS TIFF)
endif()
if(toucan_JPEG)
    list(APPEND OpenImageIO_DEPS libjpeg-turbo)
endif()
if(toucan_OpenEXR)
    list(APPEND OpenImageIO_DEPS OpenEXR)
endif()
if(toucan_Freetype)
    list(APPEND OpenImageIO_DEPS Freetype)
endif()
if(toucan_FFmpeg)
    list(APPEND OpenImageIO_DEPS FFmpeg)
endif()
if(toucan_OpenColorIO)
    list(APPEND OpenImageIO_DEPS OpenColorIO)
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
    -DUSE_FFMPEG=ON
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
    CMAKE_ARGS ${OpenImageIO_ARGS})
