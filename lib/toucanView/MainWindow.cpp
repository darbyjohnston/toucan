// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "MainWindow.h"

#include "App.h"
#include "BackgroundTool.h"
#include "CompareTool.h"
#include "DetailsTool.h"
#include "ExportTool.h"
#include "FileTab.h"
#include "FilesModel.h"
#include "GraphTool.h"
#include "InfoBar.h"
#include "JSONTool.h"
#include "LogTool.h"
#include "MenuBar.h"
#include "PlaybackBar.h"
#include "TimelineWidget.h"
#include "ToolBar.h"

#include <feather-tk/ui/DialogSystem.h>
#include <feather-tk/ui/MessageDialog.h>
#include <feather-tk/ui/Settings.h>
#include <feather-tk/core/String.h>

#include <nlohmann/json.hpp>

namespace toucan
{
    void MainWindow::_init(
        const std::shared_ptr<feather_tk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::string& name,
        const feather_tk::Size2I& size)
    {
        feather_tk::Window::_init(context, name, size);

        _app = app;
        _settings = app->getSettings();

        float displayScale = 0.F;
        try
        {
            nlohmann::json json;
            _settings->get("/MainWindow", json);
            auto i = json.find("DisplayScale");
            if (i != json.end() && i->is_number())
            {
                displayScale = i->get<float>();
            }
        }
        catch (const std::exception&)
        {}
        setDisplayScale(displayScale);

        _layout = feather_tk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(feather_tk::SizeRole::None);

        _menuBar = MenuBar::create(
            context,
            app,
            std::dynamic_pointer_cast<MainWindow>(shared_from_this()),
            _layout);
        feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, _layout);

        _toolBar = ToolBar::create(
            context,
            app,
            std::dynamic_pointer_cast<MainWindow>(shared_from_this()),
            _menuBar->getActions(),
            _layout);
        _toolBarDivider = feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, _layout);

        _vSplitter = feather_tk::Splitter::create(context, feather_tk::Orientation::Vertical, _layout);
        _vSplitter->setSplit({ .7F });
        _vSplitter->setStretch(feather_tk::Stretch::Expanding);
        _hSplitter = feather_tk::Splitter::create(context, feather_tk::Orientation::Horizontal, _vSplitter);
        _hSplitter->setSplit({ .75F });

        _tabWidget = feather_tk::TabWidget::create(context, _hSplitter);
        _tabWidget->setTabsClosable(true);
        _tabWidget->setVStretch(feather_tk::Stretch::Expanding);

        _toolWidget = feather_tk::TabWidget::create(context, _hSplitter);
        _toolWidgets.push_back(CompareTool::create(context, app));
        _toolWidgets.push_back(DetailsTool::create(context, app));
        _toolWidgets.push_back(JSONTool::create(context, app));
        _toolWidgets.push_back(GraphTool::create(context, app));
        _toolWidgets.push_back(BackgroundTool::create(context, app));
        _toolWidgets.push_back(ExportTool::create(context, app));
        _toolWidgets.push_back(LogTool::create(context, app));
        for (const auto& toolWidget : _toolWidgets)
        {
            _toolWidget->addTab(toolWidget->getText(), toolWidget);
        }

        _playbackLayout = feather_tk::VerticalLayout::create(context, _vSplitter);
        _playbackLayout->setSpacingRole(feather_tk::SizeRole::None);

        _playbackBar = PlaybackBar::create(context, app, _playbackLayout);

        auto divider = feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, _playbackLayout);

        _timelineWidget = TimelineWidget::create(context, app, _playbackLayout);
        _timelineWidget->setVStretch(feather_tk::Stretch::Expanding);

        _infoBarDivider = feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, _layout);

        _infoBar = InfoBar::create(context, app, _layout);

        std::weak_ptr<App> appWeak(app);
        _tabWidget->setTabCallback(
            [appWeak](int index)
            {
                if (auto app = appWeak.lock())
                {
                    app->getFilesModel()->setCurrentIndex(index);
                }
            });
        _tabWidget->setTabCloseCallback(
            [appWeak](int index)
            {
                if (auto app = appWeak.lock())
                {
                    app->getFilesModel()->close(index);
                }
            });

        _filesObserver = feather_tk::ListObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeFiles(),
            [this](const std::vector<std::shared_ptr<File> >& files)
            {
                _files = files;
            });

        _addObserver = feather_tk::ValueObserver<int>::create(
            app->getFilesModel()->observeAdd(),
            [this, appWeak](int index)
            {
                if (index >= 0 && index < _files.size())
                {
                    auto context = getContext();
                    auto app = appWeak.lock();
                    auto file = _files[index];
                    auto tab = FileTab::create(context, app, file);
                    _tabWidget->addTab(
                        feather_tk::elide(file->getPath().filename().string()),
                        tab,
                        file->getPath().string());
                    _fileTabs[file] = tab;
                }
            });

        _removeObserver = feather_tk::ValueObserver<int>::create(
            app->getFilesModel()->observeRemove(),
            [this, appWeak](int index)
            {
                if (index >= 0 && index < _files.size())
                {
                    auto file = _files[index];
                    const auto i = _fileTabs.find(file);
                    if (i != _fileTabs.end())
                    {
                        _tabWidget->removeTab(i->second);
                        _fileTabs.erase(i);
                    }
                }
            });

        _fileObserver = feather_tk::ValueObserver<int>::create(
            app->getFilesModel()->observeCurrentIndex(),
            [this](int index)
            {
                _tabWidget->setCurrentTab(index);
            });

        _componentsObserver = feather_tk::MapObserver<WindowComponent, bool>::create(
            app->getWindowModel()->observeComponents(),
            [this](const std::map<WindowComponent, bool>& value)
            {
                auto i = value.find(WindowComponent::ToolBar);
                _toolBar->setVisible(i->second);
                _toolBarDivider->setVisible(i->second);

                i = value.find(WindowComponent::Tools);
                _toolWidget->setVisible(i->second);

                i = value.find(WindowComponent::Playback);
                _playbackLayout->setVisible(i->second);

                i = value.find(WindowComponent::InfoBar);
                _infoBarDivider->setVisible(i->second);
                _infoBar->setVisible(i->second);
            });

        _tooltipsObserver = feather_tk::ValueObserver<bool>::create(
            app->getWindowModel()->observeTooltips(),
            [this](bool value)
            {
                setTooltipsEnabled(value);
            });
    }

    MainWindow::~MainWindow()
    {
        nlohmann::json json;
        json["DisplayScale"] = getDisplayScale();
        _settings->set("/MainWindow", json);
    }

    std::shared_ptr<MainWindow> MainWindow::create(
        const std::shared_ptr<feather_tk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::string& name,
        const feather_tk::Size2I& size)
    {
        auto out = std::shared_ptr<MainWindow>(new MainWindow);
        out->_init(context, app, name, size);
        return out;
    }

    void MainWindow::setGeometry(const feather_tk::Box2I& value)
    {
        feather_tk::Window::setGeometry(value);
        _layout->setGeometry(value);
    }

    void MainWindow::sizeHintEvent(const feather_tk::SizeHintEvent& event)
    {
        feather_tk::Window::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void MainWindow::keyPressEvent(feather_tk::KeyEvent& event)
    {
        event.accept = _menuBar->shortcut(event.key, event.modifiers);
    }

    void MainWindow::keyReleaseEvent(feather_tk::KeyEvent& event)
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
                        app->getFilesModel()->open(i);
                    }
                    catch (const std::exception& e)
                    {
                        errors.push_back(e.what());
                    }
                }
                if (!errors.empty())
                {
                    context->getSystem<feather_tk::DialogSystem>()->message(
                        "ERROR",
                        feather_tk::join(errors, '\n'),
                        getWindow());
                }
            }
        }
    }
}
