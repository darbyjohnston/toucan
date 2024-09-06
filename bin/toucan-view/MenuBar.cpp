// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "MenuBar.h"

#include "App.h"
#include "ViewModel.h"

#include <dtk/ui/Action.h>
#include <dtk/ui/FileBrowser.h>
#include <dtk/ui/MessageDialog.h>

namespace toucan
{
    void MenuBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Window>& window,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::MenuBar::_init(context, parent);

        _documentsModel = app->getDocumentsModel();

        _fileMenuInit(context, app);
        _frameMenuInit(context, app);
        _playbackMenuInit(context, app);
        _viewMenuInit(context, app);
        _windowMenuInit(context, app, window);
        _toolsMenuInit(context, app);

        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            _documentsModel->observeCurrent(),
            [this](const std::shared_ptr<Document>& document)
            {
                _document = document;
                _fileMenuUpdate();
                _frameMenuUpdate();
                _playbackMenuUpdate();
                _viewMenuUpdate();
                _windowMenuUpdate();
                _toolsMenuUpdate();
            });
    }

    MenuBar::~MenuBar()
    {}

    std::shared_ptr<MenuBar> MenuBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Window>& window,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<MenuBar>(new MenuBar);
        out->_init(context, app, window, parent);
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
            [this]
            {
                if (auto context = _getContext().lock())
                {
                    if (auto fileBrowserSystem = context->getSystem<dtk::FileBrowserSystem>())
                    {
                        fileBrowserSystem->open(
                            getWindow(),
                            [this](const std::filesystem::path& path)
                            {
                                if (auto context = _getContext().lock())
                                {
                                    try
                                    {
                                        _documentsModel->open(path);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        context->getSystem<dtk::MessageDialogSystem>()->message("ERROR", e.what(), getWindow());
                                    }
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
            [this] { _documentsModel->close(); });
        _actions["FileClose"]->toolTip = "Close the current file";
        _menus["File"]->addItem(_actions["FileClose"]);

        _actions["FileCloseAll"] = std::make_shared<dtk::Action>(
            "Close All",
            "FileCloseAll",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::KeyModifier::Control),
            [this] { _documentsModel->closeAll(); });
        _actions["FileCloseAll"]->toolTip = "Close all files";
        _menus["File"]->addItem(_actions["FileCloseAll"]);

        _menus["File"]->addDivider();

        _menus["Files"] = _menus["File"]->addSubMenu("Files");

        _actions["FileNext"] = std::make_shared<dtk::Action>(
            "Next",
            "FileNext",
            dtk::Key::PageUp,
            0,
            [this] { _documentsModel->next(); });
        _actions["FileNext"]->toolTip = "Switch to the next file";
        _menus["File"]->addItem(_actions["FileNext"]);

        _actions["FilePrev"] = std::make_shared<dtk::Action>(
            "Previous",
            "FilePrev",
            dtk::Key::PageDown,
            0,
            [this] { _documentsModel->prev(); });
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

        _documentsObserver = dtk::ListObserver<std::shared_ptr<Document> >::create(
            _documentsModel->observeDocuments(),
            [this](const std::vector<std::shared_ptr<Document> >& documents)
            {
                _menus["Files"]->clear();
                _filesActions.clear();
                for (int i = 0; i < documents.size(); ++i)
                {
                    auto item = std::make_shared<dtk::Action>(
                        documents[i]->getPath().filename().string(),
                        [this, i]
                        {
                            _documentsModel->setCurrentIndex(i);
                        });
                    _menus["Files"]->addItem(item);
                    _filesActions.push_back(item);
                }
            });

        _documentIndexObserver = dtk::ValueObserver<int>::create(
            _documentsModel->observeCurrentIndex(),
            [this](int index)
            {
                for (int i = 0; i < _filesActions.size(); ++i)
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

        _actions["FrameStart"] = std::make_shared<dtk::Action>(
            "Start Frame",
            "FrameStart",
            dtk::Key::Up,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->frameAction(FrameAction::Start);
                }
            });
        _menus["Frame"]->addItem(_actions["FrameStart"]);

        _actions["FramePrev"] = std::make_shared<dtk::Action>(
            "Previous Frame",
            "FramePrev",
            dtk::Key::Left,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->frameAction(FrameAction::Prev);
                }
            });
        _menus["Frame"]->addItem(_actions["FramePrev"]);

        _actions["FrameNext"] = std::make_shared<dtk::Action>(
            "Next Frame",
            "FrameNext",
            dtk::Key::Right,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->frameAction(FrameAction::Next);
                }
            });
        _menus["Frame"]->addItem(_actions["FrameNext"]);

        _actions["FrameEnd"] = std::make_shared<dtk::Action>(
            "End Frame",
            "FrameEnd",
            dtk::Key::Down,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->frameAction(FrameAction::End);
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

        _actions["PlaybackStop"] = std::make_shared<dtk::Action>(
            "Stop",
            "PlaybackStop",
            dtk::Key::K,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->setPlayback(Playback::Stop);
                }
            });
        _menus["Playback"]->addItem(_actions["PlaybackStop"]);

        _actions["PlaybackForward"] = std::make_shared<dtk::Action>(
            "Forward",
            "PlaybackForward",
            dtk::Key::L,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->setPlayback(Playback::Forward);
                }
            });
        _menus["Playback"]->addItem(_actions["PlaybackForward"]);

        _actions["PlaybackReverse"] = std::make_shared<dtk::Action>(
            "Reverse",
            "PlaybackReverse",
            dtk::Key::J,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->setPlayback(Playback::Reverse);
                }
            });
        _menus["Playback"]->addItem(_actions["PlaybackReverse"]);

        _menus["Playback"]->addDivider();

        _actions["TogglePlayback"] = std::make_shared<dtk::Action>(
            "Toggle Playback",
            dtk::Key::Space,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->togglePlayback();
                }
            });
        _menus["Playback"]->addItem(_actions["TogglePlayback"]);
    }

    void MenuBar::_viewMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["View"] = dtk::Menu::create(context);
        addMenu("View", _menus["View"]);

        _actions["ZoomIn"] = std::make_shared<dtk::Action>(
            "Zoom In",
            dtk::Key::Equal,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getViewModel()->zoomIn();
                }
            });
        _menus["View"]->addItem(_actions["ZoomIn"]);

        _actions["ZoomOut"] = std::make_shared<dtk::Action>(
            "Zoom Out",
            dtk::Key::Minus,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getViewModel()->zoomOut();
                }
            });
        _menus["View"]->addItem(_actions["ZoomOut"]);

        _actions["ZoomReset"] = std::make_shared<dtk::Action>(
            "Zoom Reset",
            dtk::Key::_0,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getViewModel()->zoomReset();
                }
            });
        _menus["View"]->addItem(_actions["ZoomReset"]);

        _menus["View"]->addDivider();

        _actions["FrameView"] = std::make_shared<dtk::Action>(
            "Frame View",
            dtk::Key::Backspace,
            0,
            [this](bool value)
            {
                if (_document)
                {
                    _document->getViewModel()->setFrame(value);
                }
            });
        _menus["View"]->addItem(_actions["FrameView"]);
    }

    void MenuBar::_windowMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Window>& window)
    {
        _menus["Window"] = dtk::Menu::create(context);
        addMenu("Window", _menus["Window"]);

        std::weak_ptr<Window> windowWeak(window);
        _actions["FullScreen"] = std::make_shared<dtk::Action>(
            "Full Screen",
            "WindowFullScreen",
            dtk::Key::U,
            static_cast<int>(dtk::KeyModifier::Control),
            [windowWeak](bool value)
            {
                if (auto window = windowWeak.lock())
                {
                    window->setFullScreen(value);
                }
            });
        _actions["FullScreen"]->toolTip = "Toggle full screen mode";
        _menus["Window"]->addItem(_actions["FullScreen"]);

        _fullScreenObserver = dtk::ValueObserver<bool>::create(
            window->observeFullScreen(),
            [this](bool value)
            {
                _actions["FullScreen"]->checked = value;
                _menus["Window"]->setItemChecked(_actions["FullScreen"], value);
            });
    }

    void MenuBar::_toolsMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Tools"] = dtk::Menu::create(context);
        addMenu("Tools", _menus["Tools"]);
    }

    void MenuBar::_fileMenuUpdate()
    {
        _menus["File"]->setItemEnabled(_actions["FileClose"], _document.get());
        _menus["File"]->setItemEnabled(_actions["FileCloseAll"], _document.get());
        _menus["File"]->setSubMenuEnabled(_menus["Files"], _document.get());
        _menus["File"]->setItemEnabled(_actions["FileNext"], _filesActions.size() > 1);
        _menus["File"]->setItemEnabled(_actions["FilePrev"], _filesActions.size() > 1);
    }

    void MenuBar::_frameMenuUpdate()
    {
        _menus["Frame"]->setItemEnabled(_actions["FrameStart"], _document.get());
        _menus["Frame"]->setItemEnabled(_actions["FramePrev"], _document.get());
        _menus["Frame"]->setItemEnabled(_actions["FrameNext"], _document.get());
        _menus["Frame"]->setItemEnabled(_actions["FrameEnd"], _document.get());
    }

    void MenuBar::_playbackMenuUpdate()
    {
        if (_document)
        {
            _playbackObserver = dtk::ValueObserver<Playback>::create(
                _document->getPlaybackModel()->observePlayback(),
                [this](Playback value)
                {
                    _menus["Playback"]->setItemChecked(_actions["PlaybackStop"], Playback::Stop == value);
                    _menus["Playback"]->setItemChecked(_actions["PlaybackForward"], Playback::Forward == value);
                    _menus["Playback"]->setItemChecked(_actions["PlaybackReverse"], Playback::Reverse == value);
                });
        }
        else
        {
            _playbackObserver.reset();
        }

        _menus["Playback"]->setItemEnabled(_actions["PlaybackStop"], _document.get());
        _menus["Playback"]->setItemEnabled(_actions["PlaybackForward"], _document.get());
        _menus["Playback"]->setItemEnabled(_actions["PlaybackReverse"], _document.get());
        _menus["Playback"]->setItemEnabled(_actions["TogglePlayback"], _document.get());
    }

    void MenuBar::_viewMenuUpdate()
    {
        if (_document)
        {
            _frameViewObserver = dtk::ValueObserver<bool>::create(
                _document->getViewModel()->observeFrame(),
                [this](bool value)
                {
                    _actions["FrameView"]->checked = value;
                    _menus["View"]->setItemChecked(_actions["FrameView"], value);
                });
        }
        else
        {
            _frameViewObserver.reset();
        }

        _menus["View"]->setItemEnabled(_actions["ZoomIn"], _document.get());
        _menus["View"]->setItemEnabled(_actions["ZoomOut"], _document.get());
        _menus["View"]->setItemEnabled(_actions["ZoomReset"], _document.get());
        _menus["View"]->setItemEnabled(_actions["FrameView"], _document.get());
    }

    void MenuBar::_windowMenuUpdate()
    {
    }

    void MenuBar::_toolsMenuUpdate()
    {}
}
