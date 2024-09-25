// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "App.h"

#include "Clip.h"
#include "Stack.h"
#include "Timeline.h"
#include "Track.h"

#include <dtk/core/CmdLine.h>

namespace toucan
{
    void App::_init(
        const std::shared_ptr<dtk::Context>& context,
        std::vector<std::string>& argv)
    {
        dtk::App::_init(
            context,
            argv,
            "toucan-edit",
            "Toucan editor",
            {
                dtk::CmdLineValueArg<std::string>::create(
                    _path,
                    "input",
                    "Input timeline.",
                    true)
            });

        _window = Window::create(
            context,
            std::dynamic_pointer_cast<App>(shared_from_this()),
            "toucan-edit",
            dtk::Size2I(1920, 1080));
        addWindow(_window);

        _window->show();
    }

    App::~App()
    {}

    std::shared_ptr<App> App::create(
        const std::shared_ptr<dtk::Context>& context,
        std::vector<std::string>& argv)
    {
        auto out = std::shared_ptr<App>(new App);
        out->_init(context, argv);
        return out;
    }
}
