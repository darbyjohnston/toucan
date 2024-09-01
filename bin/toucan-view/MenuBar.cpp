// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "MenuBar.h"

#include "App.h"

#include <dtkUI/Action.h>
#include <dtkUI/FileBrowser.h>

using namespace dtk;
using namespace dtk::core;
using namespace dtk::ui;

namespace toucan
{
    void MenuBar::_init(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        ui::MenuBar::_init(context, parent);

        _app = app;

        _menus["File"] = Menu::create(context);
        addMenu("File", _menus["File"]);
        _actions["FileOpen"] = std::make_shared<Action>(
            "Open",
            "FileOpen",
            Key::O,
            static_cast<int>(KeyModifier::Control),
            [this] { _fileOpenAction(); });
        _menus["File"]->addItem(_actions["FileOpen"]);
        _actions["FileClose"] = std::make_shared<Action>(
            "Close",
            "FileClose",
            Key::E,
            static_cast<int>(KeyModifier::Control),
            [this] { _fileCloseAction(); });
        _menus["File"]->addItem(_actions["FileClose"]);
        _menus["File"]->addDivider();
        _actions["Exit"] = std::make_shared<Action>(
            "Exit",
            Key::Q,
            static_cast<int>(KeyModifier::Control),
            [this] { _exitAction(); });
        _menus["File"]->addItem(_actions["Exit"]);

        _menus["Time"] = Menu::create(context);
        addMenu("Time", _menus["Time"]);
        _actions["PlaybackStop"] = std::make_shared<Action>(
            "Stop",
            "PlaybackStop",
            Key::K,
            0,
            [this] { _playbackAction(Playback::Stop); });
        _menus["Time"]->addItem(_actions["PlaybackStop"]);
        _actions["PlaybackForward"] = std::make_shared<Action>(
            "Forward",
            "PlaybackForward",
            Key::L,
            0,
            [this] { _playbackAction(Playback::Forward); });
        _menus["Time"]->addItem(_actions["PlaybackForward"]);
        _actions["PlaybackReverse"] = std::make_shared<Action>(
            "Reverse",
            "PlaybackReverse",
            Key::J,
            0,
            [this] { _playbackAction(Playback::Reverse); });
        _menus["Time"]->addItem(_actions["PlaybackReverse"]);

        _playbackObserver = ValueObserver<Playback>::create(
            app->observePlayback(),
            [this](Playback value)
            {
                _menus["Time"]->setItemChecked(_actions["PlaybackStop"], Playback::Stop == value);
                _menus["Time"]->setItemChecked(_actions["PlaybackForward"], Playback::Forward == value);
                _menus["Time"]->setItemChecked(_actions["PlaybackReverse"], Playback::Reverse == value);
            });
    }

    MenuBar::~MenuBar()
    {}

    std::shared_ptr<MenuBar> MenuBar::create(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<MenuBar>(new MenuBar);
        out->_init(context, app, parent);
        return out;
    }

    void MenuBar::_fileOpenAction()
    {
        if (auto context = _getContext().lock())
        {
            if (auto fileBrowserSystem = context->getSystem<FileBrowserSystem>())
            {
                fileBrowserSystem->open(
                    getWindow(),
                    [this](const std::filesystem::path& value)
                    {
                        if (auto app = _app.lock())
                        {
                            app->open(value);
                        }
                    });
            }
        }
    }

    void MenuBar::_fileCloseAction()
    {
        if (auto app = _app.lock())
        {
            app->close();
        }
    }

    void MenuBar::_exitAction()
    {
        if (auto app = _app.lock())
        {
            app->exit();
        }
    }

    void MenuBar::_playbackAction(Playback value)
    {
        if (auto app = _app.lock())
        {
            app->setPlayback(value);
        }
    }
}
