// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ViewMenu.h"

#include "App.h"
#include "FilesModel.h"
#include "MainWindow.h"
#include "SelectionModel.h"
#include "ViewModel.h"

#include <toucanRender/TimelineAlgo.h>

#include <dtk/ui/Action.h>
#include <dtk/ui/FileBrowser.h>
#include <dtk/ui/RecentFilesModel.h>
#include <dtk/core/Format.h>

namespace toucan
{
    void ViewMenu::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::Menu::_init(context, parent);

        _actions["View/ZoomIn"] = std::make_shared<dtk::Action>(
            "Zoom In",
            "ViewZoomIn",
            dtk::Key::Equal,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getViewModel()->zoomIn();
                }
            });
        _actions["View/ZoomIn"]->toolTip = "View zoom in";
        addItem(_actions["View/ZoomIn"]);

        _actions["View/ZoomOut"] = std::make_shared<dtk::Action>(
            "Zoom Out",
            "ViewZoomOut",
            dtk::Key::Minus,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getViewModel()->zoomOut();
                }
            });
        _actions["View/ZoomOut"]->toolTip = "View zoom out";
        addItem(_actions["View/ZoomOut"]);

        _actions["View/ZoomReset"] = std::make_shared<dtk::Action>(
            "Zoom Reset",
            "ViewZoomReset",
            dtk::Key::_0,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getViewModel()->zoomReset();
                }
            });
        _actions["View/ZoomReset"]->toolTip = "Reset the view zoom";
        addItem(_actions["View/ZoomReset"]);

        addDivider();

        _actions["View/Frame"] = std::make_shared<dtk::Action>(
            "Frame View",
            "ViewFrame",
            dtk::Key::Backspace,
            0,
            [this](bool value)
            {
                if (_file)
                {
                    _file->getViewModel()->setFrameView(value);
                }
            });
        _actions["View/Frame"]->toolTip = "Frame the view";
        addItem(_actions["View/Frame"]);

        addDivider();

        std::weak_ptr<App> appWeak(app);
        _actions["View/HUD"] = std::make_shared<dtk::Action>(
            "HUD",
            dtk::Key::H,
            static_cast<int>(dtk::KeyModifier::Control),
            [appWeak](bool value)
            {
                if (auto app = appWeak.lock())
                {
                    app->getGlobalViewModel()->setHUD(value);
                }
            });
        _actions["View/HUD"]->toolTip = "Toggle the HUD (Heads Up Display)";
        addItem(_actions["View/HUD"]);

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                _menuUpdate();
            });

        _hudObserver = dtk::ValueObserver<bool>::create(
            app->getGlobalViewModel()->observeHUD(),
            [this](bool value)
            {
                setItemChecked(_actions["View/HUD"], value);
            });
    }

    ViewMenu::~ViewMenu()
    {}

    std::shared_ptr<ViewMenu> ViewMenu::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ViewMenu>(new ViewMenu);
        out->_init(context, app, parent);
        return out;
    }

    const std::map<std::string, std::shared_ptr<dtk::Action> >& ViewMenu::getActions() const
    {
        return _actions;
    }

    void ViewMenu::_menuUpdate()
    {
        const bool file = _file.get();
        if (file)
        {
            _frameViewObserver = dtk::ValueObserver<bool>::create(
                _file->getViewModel()->observeFrameView(),
                [this](bool value)
                {
                    setItemChecked(_actions["View/Frame"], value);
                });
        }
        else
        {
            _frameViewObserver.reset();
        }

        setItemEnabled(_actions["View/ZoomIn"], file);
        setItemEnabled(_actions["View/ZoomOut"], file);
        setItemEnabled(_actions["View/ZoomReset"], file);
        setItemEnabled(_actions["View/Frame"], file);
        setItemEnabled(_actions["View/HUD"], file);
    }
}
