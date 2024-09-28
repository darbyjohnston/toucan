// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#if defined(toucan_EDIT)
#include <toucanEditTest/StackTest.h>
#endif // toucan_EDIT

#include <toucanTest/CompTest.h>
#include <toucanTest/ImageGraphTest.h>
#include <toucanTest/PropertySetTest.h>
#include <toucanTest/ReadTest.h>
#include <toucanTest/UtilTest.h>

#include <toucan/ImageEffectHost.h>

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
    searchPath.push_back(parentPath / ".." / "..");
#else // _WINDOWS
    searchPath.push_back(parentPath / "..");
#endif // _WINDOWS
    auto host = std::make_shared<ImageEffectHost>(searchPath);
    
    //compTest(path);
    //propertySetTest();
    //readTest(path);
    //imageGraphTest(path, host);
    //utilTest(path);

#if defined(toucan_EDIT)
    stackTest();
#endif // toucan_EDIT
    
    return 0;
}
