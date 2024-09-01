// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Window.h"

#include "App.h"
#include "GraphWidget.h"
#include "TimelineWidget.h"
#include "Viewport.h"

#include <dtkUI/Divider.h>

using namespace dtk;
using namespace dtk::core;
using namespace dtk::ui;

namespace toucan
{
    void Window::_init(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::string& name,
        const Size2I& size)
    {
        ui::Window::_init(context, name, size);

        _app = app;

        _layout = VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(SizeRole::None);

        _menuBar = MenuBar::create(context, app, _layout);
        Divider::create(context, Orientation::Vertical, _layout);

        _vSplitter = Splitter::create(context, Orientation::Vertical, _layout);
        _vSplitter->setSplit({ .75F, .25F });
        _hSplitter = Splitter::create(context, Orientation::Horizontal, _vSplitter);
        _hSplitter->setSplit({ .75F, .25F });
        _viewport = Viewport::create(context, app, _hSplitter);
        _graphWidget = GraphWidget::create(context, _hSplitter);
        _timelineWidget = TimelineWidget::create(context, app, _vSplitter);
    }

    Window::~Window()
    {}

    std::shared_ptr<Window> Window::create(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::string& name,
        const Size2I& size)
    {
        auto out = std::shared_ptr<Window>(new Window);
        out->_init(context, app, name, size);
        return out;
    }

    void Window::setGeometry(const Box2I& value)
    {
        ui::Window::setGeometry(value);
        _layout->setGeometry(value);
    }

    void Window::sizeHintEvent(const SizeHintEvent& event)
    {
        ui::Window::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void Window::keyPressEvent(ui::KeyEvent& event)
    {
        event.accept = _menuBar->shortcut(event.key, event.modifiers);
    }

    void Window::keyReleaseEvent(ui::KeyEvent& event)
    {
        event.accept = true;
    }
}
