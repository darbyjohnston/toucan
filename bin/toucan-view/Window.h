// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "MenuBar.h"

#include <dtkUIApp/Window.h>

#include <dtkUI/RowLayout.h>
#include <dtkUI/Splitter.h>

namespace toucan
{
    class App;
    class GraphWidget;
    class TimelineWidget;
    class Viewport;

    class Window : public dtk::ui::Window
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&,
            const std::string& name,
            const dtk::core::Size2I&);

    public:
        virtual ~Window();

        static std::shared_ptr<Window> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<App>&,
            const std::string& name,
            const dtk::core::Size2I&);

        void setGeometry(const dtk::core::Box2I&) override;
        void sizeHintEvent(const dtk::ui::SizeHintEvent&) override;
        void keyPressEvent(dtk::ui::KeyEvent&) override;
        void keyReleaseEvent(dtk::ui::KeyEvent&) override;

    private:
        std::weak_ptr<App> _app;
        std::shared_ptr<dtk::ui::VerticalLayout> _layout;
        std::shared_ptr<dtk::ui::Splitter> _vSplitter;
        std::shared_ptr<dtk::ui::Splitter> _hSplitter;
        std::shared_ptr<MenuBar> _menuBar;
        std::shared_ptr<Viewport> _viewport;
        std::shared_ptr<GraphWidget> _graphWidget;
        std::shared_ptr<TimelineWidget> _timelineWidget;
    };
}

