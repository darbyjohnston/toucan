// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "App.h"

#include <ftk/Core/Context.h>

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
        auto context = ftk::Context::create();
        auto app = App::create(context, args);
        if (!app->hasCmdLineHelp())
        {
            app->run();
        }
        out = 0;
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    return out;
}
