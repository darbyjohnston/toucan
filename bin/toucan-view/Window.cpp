// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Window.h"

#include "App.h"
#include "GraphWidget.h"
#include "TimelineWidget.h"
#include "Viewport.h"

#include <dtk/ui/Divider.h>

namespace toucan
{
    void Window::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::string& name,
        const dtk::Size2I& size)
    {
        dtk::Window::_init(context, name, size);

        _app = app;

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _menuBar = MenuBar::create(context, app, _layout);
        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _vSplitter = dtk::Splitter::create(context, dtk::Orientation::Vertical, _layout);
        _vSplitter->setSplit({ .75F, .25F });
        _hSplitter = dtk::Splitter::create(context, dtk::Orientation::Horizontal, _vSplitter);
        _hSplitter->setSplit({ .75F, .25F });
        _viewport = Viewport::create(context, app, _hSplitter);
        _graphWidget = GraphWidget::create(context, _hSplitter);
        _timelineWidget = TimelineWidget::create(context, app, _vSplitter);
    }

    Window::~Window()
    {}

    std::shared_ptr<Window> Window::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::string& name,
        const dtk::Size2I& size)
    {
        auto out = std::shared_ptr<Window>(new Window);
        out->_init(context, app, name, size);
        return out;
    }

    void Window::setGeometry(const dtk::Box2I& value)
    {
        dtk::Window::setGeometry(value);
        _layout->setGeometry(value);
    }

    void Window::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        dtk::Window::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void Window::keyPressEvent(dtk::KeyEvent& event)
    {
        event.accept = _menuBar->shortcut(event.key, event.modifiers);
    }

    void Window::keyReleaseEvent(dtk::KeyEvent& event)
    {
        event.accept = true;
    }
}
