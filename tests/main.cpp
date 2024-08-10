// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CompImageOpTest.h"
#include "ReadImageOpTest.h"
#include "TimelineTraverseTest.h"

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
    const std::filesystem::path path(argv[1]);
    init();
    compImageOpTest(path);
    readImageOpTest(path);
    timelineTraverseTest(path);
    return 0;
}
