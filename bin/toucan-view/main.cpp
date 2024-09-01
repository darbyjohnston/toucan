// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "App.h"

using namespace dtk;
using namespace dtk::core;

DTK_MAIN()
{
    try
    {
        auto context = Context::create();
        auto args = app::convert(argc, argv);
        auto app = toucan::App::create(context, args);
        if (app->getExit() != 0)
            return app->getExit();
        app->run();
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
