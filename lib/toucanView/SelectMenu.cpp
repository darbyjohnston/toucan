// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "SelectMenu.h"

#include "App.h"
#include "FilesModel.h"
#include "SelectionModel.h"

#include <dtk/ui/Action.h>

namespace toucan
{
    void SelectMenu::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::Menu::_init(context, parent);

        _actions["Select/All"] = std::make_shared<dtk::Action>(
            "All",
            dtk::Key::A,
            static_cast<int>(dtk::commandKeyModifier),
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->selectAll(_file->getTimeline());
                }
            });
        addItem(_actions["Select/All"]);

        _actions["Select/AllTracks"] = std::make_shared<dtk::Action>(
            "All Tracks",
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->selectAll(
                        _file->getTimeline(),
                        SelectionType::Tracks);
                }
            });
        addItem(_actions["Select/AllTracks"]);

        _actions["Select/AllClips"] = std::make_shared<dtk::Action>(
            "All Clips",
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->selectAll(
                        _file->getTimeline(),
                        SelectionType::Clips);
                }
            });
        addItem(_actions["Select/AllClips"]);

        _actions["Select/AllMarkers"] = std::make_shared<dtk::Action>(
            "All Markers",
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->selectAll(
                        _file->getTimeline(),
                        SelectionType::Markers);
                }
            });
        addItem(_actions["Select/AllMarkers"]);

        _actions["Select/None"] = std::make_shared<dtk::Action>(
            "None",
            dtk::Key::A,
            static_cast<int>(dtk::KeyModifier::Shift) |
            static_cast<int>(dtk::commandKeyModifier),
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->clearSelection();
                }
            });
        addItem(_actions["Select/None"]);

        _actions["Select/Invert"] = std::make_shared<dtk::Action>(
            "Invert",
            dtk::Key::I,
            static_cast<int>(dtk::commandKeyModifier),
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->invertSelection(_file->getTimeline());
                }
            });
        addItem(_actions["Select/Invert"]);

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                _menuUpdate();
            });
    }

    SelectMenu::~SelectMenu()
    {}

    std::shared_ptr<SelectMenu> SelectMenu::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<SelectMenu>(new SelectMenu);
        out->_init(context, app, parent);
        return out;
    }

    const std::map<std::string, std::shared_ptr<dtk::Action> >& SelectMenu::getActions() const
    {
        return _actions;
    }

    void SelectMenu::_menuUpdate()
    {
        const bool file = _file.get();
        setItemEnabled(_actions["Select/All"], file);
        setItemEnabled(_actions["Select/AllTracks"], file);
        setItemEnabled(_actions["Select/AllClips"], file);
        setItemEnabled(_actions["Select/AllMarkers"], file);
        setItemEnabled(_actions["Select/None"], file);
        setItemEnabled(_actions["Select/Invert"], file);
    }
}
