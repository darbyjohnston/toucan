// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#if defined(toucan_EDIT)
#include <toucanEditTest/StackTest.h>
#endif // toucan_EDIT

#include <toucanUtilTest/FileTest.h>

#include <toucanRenderTest/CompTest.h>
#include <toucanRenderTest/ImageGraphTest.h>
#include <toucanRenderTest/PropertySetTest.h>
#include <toucanRenderTest/ReadTest.h>

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
    
    std::vector<std::filesystem::path> searchPath;
    searchPath.push_back(parentPath);
#if defined(_WINDOWS)
    searchPath.push_back(parentPath / ".." / ".." / "..");
#else // _WINDOWS
    searchPath.push_back(parentPath / ".." / "..");
#endif // _WINDOWS
    auto host = std::make_shared<ImageEffectHost>(searchPath);

    fileTest(path);

    compTest(path);
    propertySetTest();
    readTest(path);
    imageGraphTest(path, host);

#if defined(toucan_EDIT)
    stackTest();
#endif // toucan_EDIT
    
    return 0;
}
