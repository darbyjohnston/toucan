// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "MenuBar.h"

#include "App.h"
#include "ViewModel.h"

#include <dtk/ui/Action.h>
#include <dtk/ui/FileBrowser.h>
#include <dtk/ui/MessageDialog.h>
#include <dtk/core/Format.h>

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
        _actions["File/Open"] = std::make_shared<dtk::Action>(
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
        _actions["File/Open"]->toolTip = "Open a file";
        _menus["File"]->addItem(_actions["File/Open"]);

        _actions["File/Close"] = std::make_shared<dtk::Action>(
            "Close",
            "FileClose",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Control),
            [this] { _documentsModel->close(); });
        _actions["File/Close"]->toolTip = "Close the current file";
        _menus["File"]->addItem(_actions["File/Close"]);

        _actions["File/CloseAll"] = std::make_shared<dtk::Action>(
            "Close All",
            "FileCloseAll",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::KeyModifier::Control),
            [this] { _documentsModel->closeAll(); });
        _actions["File/CloseAll"]->toolTip = "Close all files";
        _menus["File"]->addItem(_actions["File/CloseAll"]);

        _menus["File"]->addDivider();

        _menus["Files"] = _menus["File"]->addSubMenu("Files");

        _actions["File/Next"] = std::make_shared<dtk::Action>(
            "Next",
            "FileNext",
            dtk::Key::PageUp,
            0,
            [this] { _documentsModel->next(); });
        _actions["File/Next"]->toolTip = "Switch to the next file";
        _menus["File"]->addItem(_actions["File/Next"]);

        _actions["File/Prev"] = std::make_shared<dtk::Action>(
            "Previous",
            "FilePrev",
            dtk::Key::PageDown,
            0,
            [this] { _documentsModel->prev(); });
        _actions["File/Prev"]->toolTip = "Switch to the previous file";
        _menus["File"]->addItem(_actions["File/Prev"]);

        _menus["File"]->addDivider();

        _actions["File/Exit"] = std::make_shared<dtk::Action>(
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
        _menus["File"]->addItem(_actions["File/Exit"]);

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

        _actions["Frame/Start"] = std::make_shared<dtk::Action>(
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
        _menus["Frame"]->addItem(_actions["Frame/Start"]);

        _actions["Frame/Prev"] = std::make_shared<dtk::Action>(
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
        _menus["Frame"]->addItem(_actions["Frame/Prev"]);

        _actions["Frame/Next"] = std::make_shared<dtk::Action>(
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
        _menus["Frame"]->addItem(_actions["Frame/Next"]);

        _actions["Frame/End"] = std::make_shared<dtk::Action>(
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
        _menus["Frame"]->addItem(_actions["Frame/End"]);
    }

    void MenuBar::_playbackMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Playback"] = dtk::Menu::create(context);
        addMenu("Playback", _menus["Playback"]);

        _actions["Playback/Stop"] = std::make_shared<dtk::Action>(
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
        _menus["Playback"]->addItem(_actions["Playback/Stop"]);

        _actions["Playback/Forward"] = std::make_shared<dtk::Action>(
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
        _menus["Playback"]->addItem(_actions["Playback/Forward"]);

        _actions["Playback/Reverse"] = std::make_shared<dtk::Action>(
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
        _menus["Playback"]->addItem(_actions["Playback/Reverse"]);

        _menus["Playback"]->addDivider();

        _actions["Playback/Toggle"] = std::make_shared<dtk::Action>(
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
        _menus["Playback"]->addItem(_actions["Playback/Toggle"]);
    }

    void MenuBar::_viewMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["View"] = dtk::Menu::create(context);
        addMenu("View", _menus["View"]);

        _actions["View/ZoomIn"] = std::make_shared<dtk::Action>(
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
        _menus["View"]->addItem(_actions["View/ZoomIn"]);

        _actions["View/ZoomOut"] = std::make_shared<dtk::Action>(
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
        _menus["View"]->addItem(_actions["View/ZoomOut"]);

        _actions["View/ZoomReset"] = std::make_shared<dtk::Action>(
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
        _menus["View"]->addItem(_actions["View/ZoomReset"]);

        _menus["View"]->addDivider();

        _actions["View/FrameView"] = std::make_shared<dtk::Action>(
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
        _menus["View"]->addItem(_actions["View/FrameView"]);
    }

    void MenuBar::_windowMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Window>& window)
    {
        _menus["Window"] = dtk::Menu::create(context);
        addMenu("Window", _menus["Window"]);

        std::weak_ptr<Window> windowWeak(window);
        _actions["Window/FullScreen"] = std::make_shared<dtk::Action>(
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
        _actions["Window/FullScreen"]->toolTip = "Toggle full screen mode";
        _menus["Window"]->addItem(_actions["Window/FullScreen"]);

        _menus["Window"]->addDivider();

        struct Control
        {
            WindowControl control;
            std::string action;
            std::string text;
        };
        const std::vector<Control> controls =
        {
            { WindowControl::ToolBar, "ToolBar", "Tool Bar" },
            { WindowControl::Tabs, "Tabs", "Tabs" },
            { WindowControl::BottomBar, "BottomBar", "Bottom Bar" },
            { WindowControl::TimelineWidget, "TimelineWidget", "Timeline Widget" },
            { WindowControl::Tools, "Tools", "Tools" }
        };
        std::weak_ptr<App> appWeak(app);
        for (const auto& control : controls)
        {
            const std::string actionName = dtk::Format("Window/{0}").arg(control.action);
            _actions[actionName] = std::make_shared<dtk::Action>(
                control.text,
                [appWeak, control](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getWindowModel()->setControl(control.control, value);
                    }
                });
            _menus["Window"]->addItem(_actions[actionName]);
        }

        _fullScreenObserver = dtk::ValueObserver<bool>::create(
            window->observeFullScreen(),
            [this](bool value)
            {
                _actions["Window/FullScreen"]->checked = value;
                _menus["Window"]->setItemChecked(_actions["Window/FullScreen"], value);
            });

        _controlsObserver = dtk::MapObserver<WindowControl, bool>::create(
            app->getWindowModel()->observeControls(),
            [this](const std::map<WindowControl, bool>& value)
            {
                auto i = value.find(WindowControl::ToolBar);
                _menus["Window"]->setItemChecked(_actions["Window/ToolBar"], i->second);
                i = value.find(WindowControl::Tabs);
                _menus["Window"]->setItemChecked(_actions["Window/Tabs"], i->second);
                i = value.find(WindowControl::BottomBar);
                _menus["Window"]->setItemChecked(_actions["Window/BottomBar"], i->second);
                i = value.find(WindowControl::TimelineWidget);
                _menus["Window"]->setItemChecked(_actions["Window/TimelineWidget"], i->second);
                i = value.find(WindowControl::Tools);
                _menus["Window"]->setItemChecked(_actions["Window/Tools"], i->second);
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
        _menus["File"]->setItemEnabled(_actions["File/Close"], _document.get());
        _menus["File"]->setItemEnabled(_actions["File/CloseAll"], _document.get());
        _menus["File"]->setSubMenuEnabled(_menus["Files"], _document.get());
        _menus["File"]->setItemEnabled(_actions["File/Next"], _filesActions.size() > 1);
        _menus["File"]->setItemEnabled(_actions["File/Prev"], _filesActions.size() > 1);
    }

    void MenuBar::_frameMenuUpdate()
    {
        _menus["Frame"]->setItemEnabled(_actions["Frame/Start"], _document.get());
        _menus["Frame"]->setItemEnabled(_actions["Frame/Prev"], _document.get());
        _menus["Frame"]->setItemEnabled(_actions["Frame/Next"], _document.get());
        _menus["Frame"]->setItemEnabled(_actions["Frame/End"], _document.get());
    }

    void MenuBar::_playbackMenuUpdate()
    {
        if (_document)
        {
            _playbackObserver = dtk::ValueObserver<Playback>::create(
                _document->getPlaybackModel()->observePlayback(),
                [this](Playback value)
                {
                    _menus["Playback"]->setItemChecked(_actions["Playback/Stop"], Playback::Stop == value);
                    _menus["Playback"]->setItemChecked(_actions["Playback/Forward"], Playback::Forward == value);
                    _menus["Playback"]->setItemChecked(_actions["Playback/Reverse"], Playback::Reverse == value);
                });
        }
        else
        {
            _playbackObserver.reset();
        }

        _menus["Playback"]->setItemEnabled(_actions["Playback/Stop"], _document.get());
        _menus["Playback"]->setItemEnabled(_actions["Playback/Forward"], _document.get());
        _menus["Playback"]->setItemEnabled(_actions["Playback/Reverse"], _document.get());
        _menus["Playback"]->setItemEnabled(_actions["Playback/Toggle"], _document.get());
    }

    void MenuBar::_viewMenuUpdate()
    {
        if (_document)
        {
            _frameViewObserver = dtk::ValueObserver<bool>::create(
                _document->getViewModel()->observeFrame(),
                [this](bool value)
                {
                    _actions["View/FrameView"]->checked = value;
                    _menus["View"]->setItemChecked(_actions["View/FrameView"], value);
                });
        }
        else
        {
            _frameViewObserver.reset();
        }

        _menus["View"]->setItemEnabled(_actions["View/ZoomIn"], _document.get());
        _menus["View"]->setItemEnabled(_actions["View/ZoomOut"], _document.get());
        _menus["View"]->setItemEnabled(_actions["View/ZoomReset"], _document.get());
        _menus["View"]->setItemEnabled(_actions["View/FrameView"], _document.get());
    }

    void MenuBar::_windowMenuUpdate()
    {
    }

    void MenuBar::_toolsMenuUpdate()
    {}
}
