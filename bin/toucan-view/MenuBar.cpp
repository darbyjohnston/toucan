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
        _fileMenuInit(context, app);
        _frameMenuInit(context, app);
        _playbackMenuInit(context, app);
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

    void MenuBar::_fileMenuInit(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app)
    {
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
    }

    void MenuBar::_frameMenuInit(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Frame"] = Menu::create(context);
        addMenu("Frame", _menus["Frame"]);

        auto appWeak = std::weak_ptr<App>(app);
        _actions["FrameStart"] = std::make_shared<Action>(
            "Start Frame",
            "FrameStart",
            Key::Up,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->frameAction(FrameAction::Start);
                }
            });
        _menus["Frame"]->addItem(_actions["FrameStart"]);

        _actions["FramePrev"] = std::make_shared<Action>(
            "Previous Frame",
            "FramePrev",
            Key::Left,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->frameAction(FrameAction::Prev);
                }
            });
        _menus["Frame"]->addItem(_actions["FramePrev"]);

        _actions["FrameNext"] = std::make_shared<Action>(
            "Next Frame",
            "FrameNext",
            Key::Right,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->frameAction(FrameAction::Next);
                }
            });
        _menus["Frame"]->addItem(_actions["FrameNext"]);

        _actions["FrameEnd"] = std::make_shared<Action>(
            "End Frame",
            "FrameEnd",
            Key::Down,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->frameAction(FrameAction::End);
                }
            });
        _menus["Frame"]->addItem(_actions["FrameEnd"]);
    }

    void MenuBar::_playbackMenuInit(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Playback"] = Menu::create(context);
        addMenu("Playback", _menus["Playback"]);

        _actions["PlaybackStop"] = std::make_shared<Action>(
            "Stop",
            "PlaybackStop",
            Key::K,
            0,
            [this] { _playbackAction(Playback::Stop); });
        _menus["Playback"]->addItem(_actions["PlaybackStop"]);

        _actions["PlaybackForward"] = std::make_shared<Action>(
            "Forward",
            "PlaybackForward",
            Key::L,
            0,
            [this] { _playbackAction(Playback::Forward); });
        _menus["Playback"]->addItem(_actions["PlaybackForward"]);

        _actions["PlaybackReverse"] = std::make_shared<Action>(
            "Reverse",
            "PlaybackReverse",
            Key::J,
            0,
            [this] { _playbackAction(Playback::Reverse); });
        _menus["Playback"]->addItem(_actions["PlaybackReverse"]);

        _playbackObserver = ValueObserver<Playback>::create(
            app->getPlaybackModel()->observePlayback(),
            [this](Playback value)
            {
                _menus["Playback"]->setItemChecked(_actions["PlaybackStop"], Playback::Stop == value);
                _menus["Playback"]->setItemChecked(_actions["PlaybackForward"], Playback::Forward == value);
                _menus["Playback"]->setItemChecked(_actions["PlaybackReverse"], Playback::Reverse == value);
            });
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
            app->getFilesModel()->close();
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
            app->getPlaybackModel()->setPlayback(value);
        }
    }
}
