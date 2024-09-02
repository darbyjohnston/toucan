// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "MenuBar.h"

#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Splitter.h>
#include <dtk/ui/Window.h>

namespace toucan
{
    class App;
    class GraphWidget;
    class TimelineWidget;
    class Viewport;

    class Window : public dtk::Window
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::string& name,
            const dtk::Size2I&);

    public:
        virtual ~Window();

        static std::shared_ptr<Window> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::string& name,
            const dtk::Size2I&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void keyPressEvent(dtk::KeyEvent&) override;
        void keyReleaseEvent(dtk::KeyEvent&) override;

    private:
        std::weak_ptr<App> _app;
        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<dtk::Splitter> _vSplitter;
        std::shared_ptr<dtk::Splitter> _hSplitter;
        std::shared_ptr<MenuBar> _menuBar;
        std::shared_ptr<Viewport> _viewport;
        std::shared_ptr<GraphWidget> _graphWidget;
        std::shared_ptr<TimelineWidget> _timelineWidget;
    };
}

