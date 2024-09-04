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
        _filesModel = app->getFilesModel();
        _fileMenuInit(context, app);
        _frameMenuInit(context, app);
        _playbackMenuInit(context, app);
        _viewMenuInit(context, app);
        _windowMenuInit(context, app);
        _toolsMenuInit(context, app);
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

    const std::map<std::string, std::shared_ptr<dtk::Action> >& MenuBar::getActions() const
    {
        return _actions;
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
        _actions["FileOpen"]->toolTip = "Open a file";
        _menus["File"]->addItem(_actions["FileOpen"]);

        _actions["FileClose"] = std::make_shared<dtk::Action>(
            "Close",
            "FileClose",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Control),
            [this] { _filesModel->close(); });
        _actions["FileClose"]->toolTip = "Close the current file";
        _menus["File"]->addItem(_actions["FileClose"]);

        _menus["File"]->addDivider();

        _menus["Files"] = _menus["File"]->addSubMenu("Files");

        _actions["FileNext"] = std::make_shared<dtk::Action>(
            "Next",
            "FileNext",
            dtk::Key::PageUp,
            0,
            [this] { _filesModel->next(); });
        _actions["FileNext"]->toolTip = "Switch to the next file";
        _menus["File"]->addItem(_actions["FileNext"]);

        _actions["FilePrev"] = std::make_shared<dtk::Action>(
            "Previous",
            "FilePrev",
            dtk::Key::PageDown,
            0,
            [this] { _filesModel->prev(); });
        _actions["FilePrev"]->toolTip = "Switch to the previous file";
        _menus["File"]->addItem(_actions["FilePrev"]);

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

        _filesObserver = dtk::ListObserver<File>::create(
            _filesModel->observeFiles(),
            [this](const std::vector<File>& files)
            {
                _files = files;
                _menus["Files"]->clear();
                _filesActions.clear();
                for (int i = 0; i < files.size(); ++i)
                {
                    auto item = std::make_shared<dtk::Action>(
                        files[i].path.filename().string(),
                        [this, i]
                        {
                            if (i >= 0 && i < _files.size())
                            {
                                _filesModel->setCurrentIndex(i);
                            }
                        });
                    _menus["Files"]->addItem(item);
                    _filesActions.push_back(item);
                }
            });

        _fileObserver = dtk::ValueObserver<int>::create(
            _filesModel->observeCurrentIndex(),
            [this](int index)
            {
                for (int i = 0; i < _files.size() && i < _filesActions.size(); ++i)
                {
                    _menus["Files"]->setItemChecked(_filesActions[i], i == index);
                }
            });
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

    void MenuBar::_viewMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["View"] = dtk::Menu::create(context);
        addMenu("View", _menus["View"]);

        auto appWeak = std::weak_ptr<App>(app);
        _actions["ZoomIn"] = std::make_shared<dtk::Action>(
            "Zoom In",
            dtk::Key::Equal,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getViewModel()->zoomIn();
                }
            });
        _menus["View"]->addItem(_actions["ZoomIn"]);

        _actions["ZoomOut"] = std::make_shared<dtk::Action>(
            "Zoom Out",
            dtk::Key::Minus,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getViewModel()->zoomOut();
                }
            });
        _menus["View"]->addItem(_actions["ZoomOut"]);

        _actions["ZoomReset"] = std::make_shared<dtk::Action>(
            "Zoom Reset",
            dtk::Key::_0,
            0,
            [appWeak]
            {
                if (auto app = appWeak.lock())
                {
                    app->getViewModel()->zoomReset();
                }
            });
        _menus["View"]->addItem(_actions["ZoomReset"]);

        _menus["View"]->addDivider();

        _actions["FrameView"] = std::make_shared<dtk::Action>(
            "Frame View",
            dtk::Key::Backspace,
            0,
            [appWeak](bool value)
            {
                if (auto app = appWeak.lock())
                {
                    app->getViewModel()->setFrame(value);
                }
            });
        _menus["View"]->addItem(_actions["FrameView"]);

        _frameViewObserver = dtk::ValueObserver<bool>::create(
            app->getViewModel()->observeFrame(),
            [this](bool value)
            {
                _actions["FrameView"]->checked = value;
                _menus["View"]->setItemChecked(_actions["FrameView"], value);
            });
    }

    void MenuBar::_windowMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Window"] = dtk::Menu::create(context);
        addMenu("Window", _menus["Window"]);
    }

    void MenuBar::_toolsMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Tools"] = dtk::Menu::create(context);
        addMenu("Tools", _menus["Tools"]);
    }
}
