// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Window.h"

#include <dtk/ui/App.h>

namespace toucan
{
    class App : public dtk::App
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            std::vector<std::string>&);

    public:
        virtual ~App();

        static std::shared_ptr<App> create(
            const std::shared_ptr<dtk::Context>&,
            std::vector<std::string>&);

    private:
        std::string _path;
        std::shared_ptr<Window> _window;
    };
}

