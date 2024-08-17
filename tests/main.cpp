// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompOpTest.h"
#include "PropertySetTest.h"
#include "ReadOpTest.h"
#include "TimelineTraverseTest.h"
#include "UtilTest.h"

#include <toucan/Init.h>
#include <toucan/Host.h>

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
    auto host = std::make_shared<Host>(std::vector<std::filesystem::path>{
        parentPath,
        parentPath / ".." / ".."});
    compOpTest(path, host);
    propertySetTest();
    readOpTest(path, host);
    timelineTraverseTest(path, host);
    utilTest(path);
    return 0;
}
