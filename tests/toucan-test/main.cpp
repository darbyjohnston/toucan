// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#if defined(toucan_VIEW)
#include <toucanViewTest/FilesModelTest.h>
#include <toucanViewTest/PlaybackModelTest.h>
#include <toucanViewTest/SelectionModelTest.h>
#include <toucanViewTest/ViewModelTest.h>
#include <toucanViewTest/WindowModelTest.h>
#endif // toucan_VIEW

#include <toucanRenderTest/CompTest.h>
#include <toucanRenderTest/ImageGraphTest.h>
#include <toucanRenderTest/PropertySetTest.h>
#include <toucanRenderTest/ReadTest.h>

#if defined(toucan_VIEW)
#include <dtk/ui/Init.h>
#endif // toucan_EDIT
#include <dtk/core/Context.h>
#include <dtk/core/Init.h>

#include <iostream>

using namespace toucan;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: toucan-test (path to test data)" << std::endl;
        return 1;
    }
    const std::filesystem::path parentPath = std::filesystem::path(argv[0]).parent_path();
    const std::filesystem::path path(argv[1]);

    auto context = dtk::Context::create();
    dtk::coreInit(context);
#if defined(toucan_VIEW)
    dtk::uiInit(context);
#endif // toucan_VIEW

    std::vector<std::filesystem::path> searchPath;
    searchPath.push_back(parentPath);
#if defined(_WINDOWS)
    searchPath.push_back(parentPath / ".." / ".." / "..");
#else // _WINDOWS
    searchPath.push_back(parentPath / ".." / "..");
#endif // _WINDOWS
    auto host = std::make_shared<ImageEffectHost>(context, searchPath);

    compTest(path);
    propertySetTest();
    readTest(path);
    imageGraphTest(context, host, path);

#if defined(toucan_VIEW)
    filesModelTest(context, host, path);
    playbackModelTest(context, path);
    selectionModelTest(context, path);
    viewModelTest(context);
    windowModelTest(context);
#endif // toucan_VIEW
    
    return 0;
}
