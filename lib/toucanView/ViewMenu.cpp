// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ViewMenu.h"

#include "App.h"
#include "FilesModel.h"

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

        _actions["View/Flip"] = std::make_shared<dtk::Action>(
            "Flip Vertical",
            dtk::Key::V,
            0,
            [this](bool value)
            {
                if (_file)
                {
                    ViewOptions options = _file->getViewModel()->getOptions();
                    options.flip = value;
                    _file->getViewModel()->setOptions(options);
                }
            });
        addItem(_actions["View/Flip"]);

        _actions["View/Flop"] = std::make_shared<dtk::Action>(
            "Flop Horizontal",
            dtk::Key::H,
            0,
            [this](bool value)
            {
                if (_file)
                {
                    ViewOptions options = _file->getViewModel()->getOptions();
                    options.flop = value;
                    _file->getViewModel()->setOptions(options);
                }
            });
        addItem(_actions["View/Flop"]);

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

            _optionsObserver = dtk::ValueObserver<ViewOptions>::create(
                _file->getViewModel()->observeOptions(),
                [this](const ViewOptions& value)
                {
                    setItemChecked(_actions["View/Flip"], value.flip);
                    setItemChecked(_actions["View/Flop"], value.flop);
                });
        }
        else
        {
            setItemChecked(_actions["View/Frame"], false);
            setItemChecked(_actions["View/Flip"], false);
            setItemChecked(_actions["View/Flop"], false);
            _frameViewObserver.reset();
            _optionsObserver.reset();
        }

        setItemEnabled(_actions["View/ZoomIn"], file);
        setItemEnabled(_actions["View/ZoomOut"], file);
        setItemEnabled(_actions["View/ZoomReset"], file);
        setItemEnabled(_actions["View/Frame"], file);
        setItemEnabled(_actions["View/Flip"], file);
        setItemEnabled(_actions["View/Flop"], file);
        setItemEnabled(_actions["View/HUD"], file);
    }
}
