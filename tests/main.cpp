// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompTest.h"
#include "PropertySetTest.h"
#include "ReadTest.h"
#include "TimelineGraphTest.h"
#include "UtilTest.h"

#include <toucan/ImageEffectHost.h>
#include <toucan/Init.h>

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

    init();
    
    std::vector<std::filesystem::path> searchPath;
    searchPath.push_back(parentPath);
#if defined(_WINDOWS)
    searchPath.push_back(parentPath / ".." / "..");
#else // _WINDOWS
    searchPath.push_back(parentPath / "..");
#endif // _WINDOWS
    auto host = std::make_shared<ImageEffectHost>(searchPath);
    
    compTest(path, host);
    propertySetTest();
    readTest(path, host);
    timelineGraphTest(path, host);
    utilTest(path);
    
    return 0;
}
