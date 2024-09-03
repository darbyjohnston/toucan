// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "MenuBar.h"

#include "App.h"

#include <dtk/ui/Action.h>
#include <dtk/ui/FileBrowser.h>

namespace toucan
{
    void MenuBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::MenuBar::_init(context, parent);
        _fileMenuInit(context, app);
        _frameMenuInit(context, app);
        _playbackMenuInit(context, app);
    }

    MenuBar::~MenuBar()
    {}

    std::shared_ptr<MenuBar> MenuBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<MenuBar>(new MenuBar);
        out->_init(context, app, parent);
        return out;
    }

    void MenuBar::_fileMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["File"] = dtk::Menu::create(context);
        addMenu("File", _menus["File"]);

        auto appWeak = std::weak_ptr<App>(app);
        _actions["FileOpen"] = std::make_shared<dtk::Action>(
            "Open",
            "FileOpen",
            dtk::Key::O,
            static_cast<int>(dtk::KeyModifier::Control),
            [this, appWeak]
            {
                if (auto context = _getContext().lock())
                {
                    if (auto fileBrowserSystem = context->getSystem<dtk::FileBrowserSystem>())
                    {
                        fileBrowserSystem->open(
                            getWindow(),
                            [appWeak](const std::filesystem::path& value)
                            {
                                if (auto app = appWeak.lock())
                                {
                                    app->open(value);
                                }
                            });
                    }
                }
            });
        _menus["File"]->addItem(_actions["FileOpen"]);

        _actions["FileClose"] = std::make_shared<dtk::Action>(
            "Close",
            "FileClose",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Control),
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getFilesModel()->close();
                }
            });
        _menus["File"]->addItem(_actions["FileClose"]);

        _menus["File"]->addDivider();

        _actions["Exit"] = std::make_shared<dtk::Action>(
            "Exit",
            dtk::Key::Q,
            static_cast<int>(dtk::KeyModifier::Control),
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->exit();
                }
            });
        _menus["File"]->addItem(_actions["Exit"]);
    }

    void MenuBar::_frameMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Frame"] = dtk::Menu::create(context);
        addMenu("Frame", _menus["Frame"]);

        auto appWeak = std::weak_ptr<App>(app);
        _actions["FrameStart"] = std::make_shared<dtk::Action>(
            "Start Frame",
            "FrameStart",
            dtk::Key::Up,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->frameAction(FrameAction::Start);
                }
            });
        _menus["Frame"]->addItem(_actions["FrameStart"]);

        _actions["FramePrev"] = std::make_shared<dtk::Action>(
            "Previous Frame",
            "FramePrev",
            dtk::Key::Left,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->frameAction(FrameAction::Prev);
                }
            });
        _menus["Frame"]->addItem(_actions["FramePrev"]);

        _actions["FrameNext"] = std::make_shared<dtk::Action>(
            "Next Frame",
            "FrameNext",
            dtk::Key::Right,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->frameAction(FrameAction::Next);
                }
            });
        _menus["Frame"]->addItem(_actions["FrameNext"]);

        _actions["FrameEnd"] = std::make_shared<dtk::Action>(
            "End Frame",
            "FrameEnd",
            dtk::Key::Down,
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
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Playback"] = dtk::Menu::create(context);
        addMenu("Playback", _menus["Playback"]);

        auto appWeak = std::weak_ptr<App>(app);
        _actions["PlaybackStop"] = std::make_shared<dtk::Action>(
            "Stop",
            "PlaybackStop",
            dtk::Key::K,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->setPlayback(Playback::Stop);
                }
            });
        _menus["Playback"]->addItem(_actions["PlaybackStop"]);

        _actions["PlaybackForward"] = std::make_shared<dtk::Action>(
            "Forward",
            "PlaybackForward",
            dtk::Key::L,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->setPlayback(Playback::Forward);
                }
            });
        _menus["Playback"]->addItem(_actions["PlaybackForward"]);

        _actions["PlaybackReverse"] = std::make_shared<dtk::Action>(
            "Reverse",
            "PlaybackReverse",
            dtk::Key::J,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->setPlayback(Playback::Reverse);
                }
            });
        _menus["Playback"]->addItem(_actions["PlaybackReverse"]);

        _menus["Playback"]->addDivider();

        _actions["TogglePlayback"] = std::make_shared<dtk::Action>(
            "Toggle Playback",
            dtk::Key::Space,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getPlaybackModel()->togglePlayback();
                }
            });
        _menus["Playback"]->addItem(_actions["TogglePlayback"]);

        _playbackObserver = dtk::ValueObserver<Playback>::create(
            app->getPlaybackModel()->observePlayback(),
            [this](Playback value)
            {
                _menus["Playback"]->setItemChecked(_actions["PlaybackStop"], Playback::Stop == value);
                _menus["Playback"]->setItemChecked(_actions["PlaybackForward"], Playback::Forward == value);
                _menus["Playback"]->setItemChecked(_actions["PlaybackReverse"], Playback::Reverse == value);
            });
    }
}
