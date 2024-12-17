// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "App.h"

#include <dtk/core/Context.h>
#include <dtk/core/Init.h>

#include <iostream>

using namespace toucan;

int main(int argc, char** argv)
{
    int out = 1;
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    try
    {
        auto context = dtk::Context::create();
        dtk::coreInit(context);
        auto app = App::create(context, args);
        app->run();
        out = app->getExit();
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    return out;
}
