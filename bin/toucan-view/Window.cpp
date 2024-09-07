// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Window.h"

#include "App.h"
#include "BottomBar.h"
#include "GraphWidget.h"
#include "MenuBar.h"
#include "TimelineWidget.h"
#include "ToolBar.h"
#include "Viewport.h"

#include <dtk/ui/MessageDialog.h>
#include <dtk/core/String.h>

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

        _menuBar = MenuBar::create(
            context,
            app,
            std::dynamic_pointer_cast<Window>(shared_from_this()),
            _layout);
        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _toolBar = ToolBar::create(
            context,
            app,
            std::dynamic_pointer_cast<Window>(shared_from_this()),
            _menuBar->getActions(),
            _layout);
        _toolBarDivider = dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _vSplitter = dtk::Splitter::create(context, dtk::Orientation::Vertical, _layout);
        _vSplitter->setSplit({ .75F, .25F });
        _vSplitter->setStretch(dtk::Stretch::Expanding);
        _hSplitter = dtk::Splitter::create(context, dtk::Orientation::Horizontal, _vSplitter);
        _hSplitter->setSplit({ .75F, .25F });
        auto vLayout = dtk::VerticalLayout::create(context, _hSplitter);
        vLayout->setSpacingRole(dtk::SizeRole::None);
        _tabBar = dtk::TabBar::create(context, vLayout);
        _viewport = Viewport::create(context, app, vLayout);
        _viewport->setStretch(dtk::Stretch::Expanding);
        _graphWidget = GraphWidget::create(context, _hSplitter);

        _bottomLayout = dtk::VerticalLayout::create(context, _vSplitter);
        _bottomLayout->setSpacingRole(dtk::SizeRole::None);
        _bottomBar = BottomBar::create(context, app, _bottomLayout);
        _timelineWidget = TimelineWidget::create(context, app, _bottomLayout);
        _timelineWidget->setVStretch(dtk::Stretch::Expanding);

        std::weak_ptr<App> appWeak(app);
        _tabBar->setCallback(
            [appWeak](int index)
            {
                if (auto app = appWeak.lock())
                {
                    app->getDocumentsModel()->setCurrentIndex(index);
                }
            });

        _documentsObserver = dtk::ListObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeDocuments(),
            [this](const std::vector<std::shared_ptr<Document> >& documents)
            {
                _tabBar->clearTabs();
                for (const auto& document : documents)
                {
                    _tabBar->addTab(
                        dtk::elide(document->getPath().filename().string()),
                        document->getPath().string());
                }
                _tabBar->setVisible(documents.size());
            });

        _documentObserver = dtk::ValueObserver<int>::create(
            app->getDocumentsModel()->observeCurrentIndex(),
            [this](int index)
            {
                _tabBar->setCurrentTab(index);
            });

        _controlsObserver = dtk::MapObserver<WindowControl, bool>::create(
            app->getWindowModel()->observeControls(),
            [this](const std::map<WindowControl, bool>& value)
            {
                auto i = value.find(WindowControl::ToolBar);
                _toolBar->setVisible(i->second);
                _toolBarDivider->setVisible(i->second);
                i = value.find(WindowControl::Tabs);
                _tabBar->setVisible(i->second);
                i = value.find(WindowControl::BottomBar);
                _bottomBar->setVisible(i->second);
                auto j = value.find(WindowControl::TimelineWidget);
                _timelineWidget->setVisible(j->second);
                _bottomLayout->setVisible(i->second || j->second);
                i = value.find(WindowControl::Tools);
                _graphWidget->setVisible(i->second);
            });
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

    void Window::_drop(const std::vector<std::string>& value)
    {
        if (auto context = _getContext().lock())
        {
            if (auto app = _app.lock())
            {
                std::vector<std::string> errors;
                for (const auto& i : value)
                {
                    try
                    {
                        app->getDocumentsModel()->open(i);
                    }
                    catch (const std::exception& e)
                    {
                        errors.push_back(e.what());
                    }
                }
                if (!errors.empty())
                {
                    context->getSystem<dtk::MessageDialogSystem>()->message(
                        "ERROR",
                        dtk::join(errors, '\n'),
                        getWindow());
                }
            }
        }
    }
}
