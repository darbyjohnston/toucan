// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "MainWindow.h"

#include "App.h"
#include "DocumentTab.h"
#include "DocumentsModel.h"
#include "ExportTool.h"
#include "GraphTool.h"
#include "InfoBar.h"
#include "JSONTool.h"
#include "MenuBar.h"
#include "PlaybackBar.h"
#include "TimelineWidget.h"
#include "ToolBar.h"

#include <dtk/ui/DialogSystem.h>
#include <dtk/ui/MessageDialog.h>
#include <dtk/core/String.h>

namespace toucan
{
    void MainWindow::_init(
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
            std::dynamic_pointer_cast<MainWindow>(shared_from_this()),
            _layout);
        dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _toolBar = ToolBar::create(
            context,
            app,
            std::dynamic_pointer_cast<MainWindow>(shared_from_this()),
            _menuBar->getActions(),
            _layout);
        _toolBarDivider = dtk::Divider::create(context, dtk::Orientation::Vertical, _layout);

        _vSplitter = dtk::Splitter::create(context, dtk::Orientation::Vertical, _layout);
        _vSplitter->setSplit({ .7F, .3F });
        _vSplitter->setStretch(dtk::Stretch::Expanding);
        _hSplitter = dtk::Splitter::create(context, dtk::Orientation::Horizontal, _vSplitter);
        _hSplitter->setSplit({ .75F, .25F });

        _tabWidget = dtk::TabWidget::create(context, _hSplitter);
        _tabWidget->setTabsClosable(true);
        _tabWidget->setVStretch(dtk::Stretch::Expanding);

        _toolWidget = dtk::TabWidget::create(context, _hSplitter);
        _toolWidgets.push_back(JSONTool::create(context, app));
        _toolWidgets.push_back(GraphTool::create(context, app));
        _toolWidgets.push_back(ExportTool::create(context, app));
        for (const auto& toolWidget : _toolWidgets)
        {
            _toolWidget->addTab(toolWidget->getText(), toolWidget);
        }

        _bottomLayout = dtk::VerticalLayout::create(context, _vSplitter);
        _bottomLayout->setSpacingRole(dtk::SizeRole::None);

        _playbackBar = PlaybackBar::create(context, app, _bottomLayout);

        _timelineDivider = dtk::Divider::create(context, dtk::Orientation::Vertical, _bottomLayout);

        _timelineWidget = TimelineWidget::create(context, app, _bottomLayout);
        _timelineWidget->setVStretch(dtk::Stretch::Expanding);

        _infoDivider = dtk::Divider::create(context, dtk::Orientation::Vertical, _bottomLayout);

        _infoBar = InfoBar::create(context, app, _bottomLayout);

        std::weak_ptr<App> appWeak(app);
        _tabWidget->setCallback(
            [appWeak](int index)
            {
                if (auto app = appWeak.lock())
                {
                    app->getDocumentsModel()->setCurrentIndex(index);
                }
            });
        _tabWidget->setTabCloseCallback(
            [appWeak](int index)
            {
                if (auto app = appWeak.lock())
                {
                    app->getDocumentsModel()->close(index);
                }
            });

        _documentsObserver = dtk::ListObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeDocuments(),
            [this](const std::vector<std::shared_ptr<Document> >& documents)
            {
                _documents = documents;
            });

        _addObserver = dtk::ValueObserver<int>::create(
            app->getDocumentsModel()->observeAdd(),
            [this, appWeak](int index)
            {
                if (index >= 0 && index < _documents.size())
                {
                    auto context = getContext();
                    auto app = appWeak.lock();
                    auto document = _documents[index];
                    auto tab = DocumentTab::create(context, app, document);
                    _tabWidget->addTab(
                        dtk::elide(document->getPath().filename().string()),
                        tab,
                        document->getPath().string());
                    _documentTabs[document] = tab;
                }
            });

        _removeObserver = dtk::ValueObserver<int>::create(
            app->getDocumentsModel()->observeRemove(),
            [this, appWeak](int index)
            {
                if (index >= 0 && index < _documents.size())
                {
                    auto document = _documents[index];
                    const auto i = _documentTabs.find(document);
                    if (i != _documentTabs.end())
                    {
                        _tabWidget->removeTab(i->second);
                        _documentTabs.erase(i);
                    }
                }
            });

        _documentObserver = dtk::ValueObserver<int>::create(
            app->getDocumentsModel()->observeCurrentIndex(),
            [this](int index)
            {
                _tabWidget->setCurrentTab(index);
            });

        _controlsObserver = dtk::MapObserver<WindowControl, bool>::create(
            app->getWindowModel()->observeControls(),
            [this](const std::map<WindowControl, bool>& value)
            {
                auto i = value.find(WindowControl::ToolBar);
                _toolBar->setVisible(i->second);
                _toolBarDivider->setVisible(i->second);

                i = value.find(WindowControl::PlaybackBar);
                _playbackBar->setVisible(i->second);
                auto j = value.find(WindowControl::TimelineWidget);
                _timelineWidget->setVisible(j->second);
                auto k = value.find(WindowControl::InfoBar);
                _infoBar->setVisible(k->second);
                _bottomLayout->setVisible(i->second || j->second || k->second);
                _timelineDivider->setVisible(i->second && j->second);
                _infoDivider->setVisible((i->second || j->second) && k->second);

                i = value.find(WindowControl::Tools);
                _toolWidget->setVisible(i->second);
            });

        _tooltipsObserver = dtk::ValueObserver<bool>::create(
            app->getWindowModel()->observeTooltips(),
            [this](bool value)
            {
                setTooltipsEnabled(value);
            });
    }

    MainWindow::~MainWindow()
    {}

    std::shared_ptr<MainWindow> MainWindow::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::string& name,
        const dtk::Size2I& size)
    {
        auto out = std::shared_ptr<MainWindow>(new MainWindow);
        out->_init(context, app, name, size);
        return out;
    }

    void MainWindow::setGeometry(const dtk::Box2I& value)
    {
        dtk::Window::setGeometry(value);
        _layout->setGeometry(value);
    }

    void MainWindow::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        dtk::Window::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void MainWindow::keyPressEvent(dtk::KeyEvent& event)
    {
        event.accept = _menuBar->shortcut(event.key, event.modifiers);
    }

    void MainWindow::keyReleaseEvent(dtk::KeyEvent& event)
    {
        event.accept = true;
    }

    void MainWindow::_drop(const std::vector<std::string>& value)
    {
        if (auto context = getContext())
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
                    context->getSystem<dtk::DialogSystem>()->message(
                        "ERROR",
                        dtk::join(errors, '\n'),
                        getWindow());
                }
            }
        }
    }
}
