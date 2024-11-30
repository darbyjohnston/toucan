// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "MenuBar.h"

#include "App.h"
#include "DocumentsModel.h"
#include "MainWindow.h"
#include "SelectionModel.h"
#include "ViewModel.h"

#include <toucan/TimelineAlgo.h>

#include <dtk/ui/Action.h>
#include <dtk/ui/DialogSystem.h>
#include <dtk/ui/FileBrowser.h>
#include <dtk/ui/MessageDialog.h>
#include <dtk/ui/RecentFilesModel.h>
#include <dtk/core/Format.h>

namespace toucan
{
    void MenuBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& window,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::MenuBar::_init(context, parent);

        _app = app;
        _documentsModel = app->getDocumentsModel();

        _fileMenuInit(context, app);
        _selectMenuInit(context, app);
        _timeMenuInit(context, app);
        _playbackMenuInit(context, app);
        _windowMenuInit(context, app, window);
        _viewMenuInit(context, app);

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
                            _menus["File"]->close();
                        });
                    _menus["Files"]->addItem(item);
                    _filesActions.push_back(item);
                }
            });

        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            _documentsModel->observeCurrent(),
            [this](const std::shared_ptr<Document>& document)
            {
                _document = document;
                _fileMenuUpdate();
                _selectMenuUpdate();
                _timeMenuUpdate();
                _playbackMenuUpdate();
                _windowMenuUpdate();
                _viewMenuUpdate();
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

        _recentFilesObserver = dtk::ListObserver<std::filesystem::path>::create(
            _documentsModel->getRecentFilesModel()->observeRecent(),
            [this](const std::vector<std::filesystem::path>& files)
            {
                _menus["RecentFiles"]->clear();
                _recentFilesActions.clear();
                for (auto i = files.rbegin(); i != files.rend(); ++i)
                {
                    auto file = *i;
                    auto item = std::make_shared<dtk::Action>(
                        file.string(),
                        [this, file]
                        {
                            _documentsModel->open(file);
                            _menus["File"]->close();
                        });
                    _menus["RecentFiles"]->addItem(item);
                    _recentFilesActions.push_back(item);
                }
            });
    }

    MenuBar::~MenuBar()
    {}

    std::shared_ptr<MenuBar> MenuBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& window,
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
                if (auto context = getContext())
                {
                    if (auto fileBrowserSystem = context->getSystem<dtk::FileBrowserSystem>())
                    {
                        if (_document)
                        {
                            fileBrowserSystem->setPath(_document->getPath().parent_path());
                        }
                        dtk::FileBrowserOptions options;
                        options.extensions.push_back(".otio");
                        fileBrowserSystem->setOptions(options);
                        fileBrowserSystem->open(
                            getWindow(),
                            [this](const std::filesystem::path& path)
                            {
                                if (auto context = getContext())
                                {
                                    try
                                    {
                                        _documentsModel->open(path);
                                    }
                                    catch (const std::exception& e)
                                    {
                                        context->getSystem<dtk::DialogSystem>()->message("ERROR", e.what(), getWindow());
                                    }
                                }
                            },
                            _documentsModel->getRecentFilesModel());
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

        _menus["RecentFiles"] = _menus["File"]->addSubMenu("RecentFiles");

        _menus["File"]->addDivider();

        _menus["Files"] = _menus["File"]->addSubMenu("Files");

        _actions["File/Next"] = std::make_shared<dtk::Action>(
            "Next",
            dtk::Key::PageUp,
            0,
            [this] { _documentsModel->next(); });
        _actions["File/Next"]->toolTip = "Switch to the next file";
        _menus["File"]->addItem(_actions["File/Next"]);

        _actions["File/Prev"] = std::make_shared<dtk::Action>(
            "Previous",
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
    }

    void MenuBar::_selectMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Select"] = dtk::Menu::create(context);
        addMenu("Select", _menus["Select"]);

        _actions["Select/All"] = std::make_shared<dtk::Action>(
            "All",
            dtk::Key::A,
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_document)
                {
                    _document->getSelectionModel()->selectAll(_document->getTimeline());
                }
            });
        _menus["Select"]->addItem(_actions["Select/All"]);

        _actions["Select/AllTracks"] = std::make_shared<dtk::Action>(
            "All Tracks",
            [this]
            {
                if (_document)
                {
                    _document->getSelectionModel()->selectAll(
                        _document->getTimeline(),
                        SelectionType::Tracks);
                }
            });
        _menus["Select"]->addItem(_actions["Select/AllTracks"]);

        _actions["Select/AllClips"] = std::make_shared<dtk::Action>(
            "All Clips",
            [this]
            {
                if (_document)
                {
                    _document->getSelectionModel()->selectAll(
                        _document->getTimeline(),
                        SelectionType::Clips);
                }
            });
        _menus["Select"]->addItem(_actions["Select/AllClips"]);

        _actions["Select/None"] = std::make_shared<dtk::Action>(
            "None",
            dtk::Key::A,
            static_cast<int>(dtk::KeyModifier::Shift) |
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_document)
                {
                    _document->getSelectionModel()->clearSelection();
                }
            });
        _menus["Select"]->addItem(_actions["Select/None"]);

        _actions["Select/Invert"] = std::make_shared<dtk::Action>(
            "Invert",
            dtk::Key::I,
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_document)
                {
                    _document->getSelectionModel()->invertSelection(_document->getTimeline());
                }
            });
        _menus["Select"]->addItem(_actions["Select/Invert"]);
    }

    void MenuBar::_timeMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["Time"] = dtk::Menu::create(context);
        addMenu("Time", _menus["Time"]);

        _actions["Time/FrameStart"] = std::make_shared<dtk::Action>(
            "Start Frame",
            "FrameStart",
            dtk::Key::Up,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->timeAction(
                        TimeAction::FrameStart,
                        _document->getTimeline());
                }
            });
        _menus["Time"]->addItem(_actions["Time/FrameStart"]);

        _actions["Time/FramePrev"] = std::make_shared<dtk::Action>(
            "Previous Frame",
            "FramePrev",
            dtk::Key::Left,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->timeAction(
                        TimeAction::FramePrev,
                        _document->getTimeline());
                }
            });
        _menus["Time"]->addItem(_actions["Time/FramePrev"]);

        _actions["Time/FrameNext"] = std::make_shared<dtk::Action>(
            "Next Frame",
            "FrameNext",
            dtk::Key::Right,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->timeAction(
                        TimeAction::FrameNext,
                        _document->getTimeline());
                }
            });
        _menus["Time"]->addItem(_actions["Time/FrameNext"]);

        _actions["Time/FrameEnd"] = std::make_shared<dtk::Action>(
            "End Frame",
            "FrameEnd",
            dtk::Key::Down,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->timeAction(
                        TimeAction::FrameEnd,
                        _document->getTimeline());
                }
            });
        _menus["Time"]->addItem(_actions["Time/FrameEnd"]);

        _menus["Time"]->addDivider();

        _actions["Time/ClipNext"] = std::make_shared<dtk::Action>(
            "Next Clip",
            dtk::Key::Right,
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->timeAction(
                        TimeAction::ClipNext,
                        _document->getTimeline());
                }
            });
        _menus["Time"]->addItem(_actions["Time/ClipNext"]);

        _actions["Time/ClipPrev"] = std::make_shared<dtk::Action>(
            "Previous Clip",
            dtk::Key::Left,
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->timeAction(
                        TimeAction::ClipPrev,
                        _document->getTimeline());
                }
            });
        _menus["Time"]->addItem(_actions["Time/ClipPrev"]);

        _menus["Time"]->addDivider();

        _actions["Time/InPointSet"] = std::make_shared<dtk::Action>(
            "Set In Point",
            dtk::Key::I,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->setInPoint(
                        _document->getPlaybackModel()->getCurrentTime());
                }
            });
        _menus["Time"]->addItem(_actions["Time/InPointSet"]);

        _actions["Time/InPointReset"] = std::make_shared<dtk::Action>(
            "Reset In Point",
            dtk::Key::I,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->resetInPoint();
                }
            });
        _menus["Time"]->addItem(_actions["Time/InPointReset"]);

        _actions["Time/OutPointSet"] = std::make_shared<dtk::Action>(
            "Set Out Point",
            dtk::Key::O,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->setOutPoint(
                        _document->getPlaybackModel()->getCurrentTime());
                }
            });
        _menus["Time"]->addItem(_actions["Time/OutPointSet"]);

        _actions["Time/OutPointReset"] = std::make_shared<dtk::Action>(
            "Reset Out Point",
            dtk::Key::O,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->resetOutPoint();
                }
            });
        _menus["Time"]->addItem(_actions["Time/OutPointReset"]);

        _actions["Time/InOutPointReset"] = std::make_shared<dtk::Action>(
            "Reset In/Out Points",
            dtk::Key::P,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_document)
                {
                    _document->getPlaybackModel()->resetInOutPoints();
                }
            });
        _menus["Time"]->addItem(_actions["Time/InOutPointReset"]);

        _actions["Time/InOutPointSelection"] = std::make_shared<dtk::Action>(
            "Set In/Out Points To Selection",
            dtk::Key::P,
            static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_document)
                {
                    const auto selection = _document->getSelectionModel()->getSelection();
                    const OTIO_NS::TimeRange& timeRange = _document->getTimelineWrapper()->getTimeRange();
                    const auto timeRangeOpt = getTimeRange(
                        selection,
                        timeRange.start_time(),
                        timeRange.duration().rate());
                    if (timeRangeOpt.has_value())
                    {
                        _document->getPlaybackModel()->setInOutRange(timeRangeOpt.value());
                    }
                    else
                    {
                        _document->getPlaybackModel()->resetInOutPoints();
                    }
                }
            });
        _menus["Time"]->addItem(_actions["Time/InOutPointSelection"]);
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

    void MenuBar::_windowMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& window)
    {
        _menus["Window"] = dtk::Menu::create(context);
        addMenu("Window", _menus["Window"]);

        std::weak_ptr<MainWindow> windowWeak(window);
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
            { WindowControl::PlaybackBar, "PlaybackBar", "Playback Bar" },
            { WindowControl::TimelineWidget, "TimelineWidget", "Timeline Widget" },
            { WindowControl::InfoBar, "InfoBar", "Information Bar" },
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

        _menus["Window"]->addDivider();

        _menus["Window/Resize"] = _menus["Window"]->addSubMenu("Resize");

        _actions["Window/Resize/1280x720"] = std::make_shared<dtk::Action>(
            "1280x720",
            [windowWeak]
            {
                if (auto window = windowWeak.lock())
                {
                    window->setSize(dtk::Size2I(1280, 720));
                }
            });
        _menus["Window/Resize"]->addItem(_actions["Window/Resize/1280x720"]);

        _actions["Window/Resize/1920x1080"] = std::make_shared<dtk::Action>(
            "1920x1080",
            [windowWeak]
            {
                if (auto window = windowWeak.lock())
                {
                    window->setSize(dtk::Size2I(1920, 1080));
                }
            });
        _menus["Window/Resize"]->addItem(_actions["Window/Resize/1920x1080"]);

        _actions["Window/Resize/3840x2160"] = std::make_shared<dtk::Action>(
            "3840x2160",
            [windowWeak]
            {
                if (auto window = windowWeak.lock())
                {
                    window->setSize(dtk::Size2I(3840, 2160));
                }
            });
        _menus["Window/Resize"]->addItem(_actions["Window/Resize/3840x2160"]);

        _menus["Window/DisplayScale"] = _menus["Window"]->addSubMenu("Display Scale");

        _actions["Window/DisplayScale/Auto"] = std::make_shared<dtk::Action>(
            "Automatic",
            [windowWeak](bool value)
            {
                if (auto window = windowWeak.lock())
                {
                    window->setDisplayScale(0.F);
                }
            });
        _menus["Window/DisplayScale"]->addItem(_actions["Window/DisplayScale/Auto"]);

        _actions["Window/DisplayScale/1.0"] = std::make_shared<dtk::Action>(
            "1.0",
            [windowWeak](bool value)
            {
                if (auto window = windowWeak.lock())
                {
                    window->setDisplayScale(1.F);
                }
            });
        _menus["Window/DisplayScale"]->addItem(_actions["Window/DisplayScale/1.0"]);

        _actions["Window/DisplayScale/2.0"] = std::make_shared<dtk::Action>(
            "2.0",
            [windowWeak](bool value)
            {
                if (auto window = windowWeak.lock())
                {
                    window->setDisplayScale(2.F);
                }
            });
        _menus["Window/DisplayScale"]->addItem(_actions["Window/DisplayScale/2.0"]);

        _actions["Window/DisplayScale/3.0"] = std::make_shared<dtk::Action>(
            "3.0",
            [windowWeak](bool value)
            {
                if (auto window = windowWeak.lock())
                {
                    window->setDisplayScale(3.F);
                }
            });
        _menus["Window/DisplayScale"]->addItem(_actions["Window/DisplayScale/3.0"]);

        _actions["Window/Tooltips"] = std::make_shared<dtk::Action>(
            "Tooltips",
            [appWeak](bool value)
            {
                if (auto app = appWeak.lock())
                {
                    app->getWindowModel()->setTooltips(value);
                }
            });
        _menus["Window"]->addItem(_actions["Window/Tooltips"]);

        _fullScreenObserver = dtk::ValueObserver<bool>::create(
            window->observeFullScreen(),
            [this](bool value)
            {
                _menus["Window"]->setItemChecked(_actions["Window/FullScreen"], value);
            });

        _controlsObserver = dtk::MapObserver<WindowControl, bool>::create(
            app->getWindowModel()->observeControls(),
            [this](const std::map<WindowControl, bool>& value)
            {
                auto i = value.find(WindowControl::ToolBar);
                _menus["Window"]->setItemChecked(_actions["Window/ToolBar"], i->second);
                i = value.find(WindowControl::PlaybackBar);
                _menus["Window"]->setItemChecked(_actions["Window/PlaybackBar"], i->second);
                i = value.find(WindowControl::TimelineWidget);
                _menus["Window"]->setItemChecked(_actions["Window/TimelineWidget"], i->second);
                i = value.find(WindowControl::InfoBar);
                _menus["Window"]->setItemChecked(_actions["Window/InfoBar"], i->second);
                i = value.find(WindowControl::Tools);
                _menus["Window"]->setItemChecked(_actions["Window/Tools"], i->second);
            });

        _displayScaleObserver = dtk::ValueObserver<float>::create(
            window->observeDisplayScale(),
            [this](float value)
            {
                _menus["Window/DisplayScale"]->setItemChecked(_actions["Window/DisplayScale/Auto"], 0.F == value);
                _menus["Window/DisplayScale"]->setItemChecked(_actions["Window/DisplayScale/1.0"], 1.F == value);
                _menus["Window/DisplayScale"]->setItemChecked(_actions["Window/DisplayScale/2.0"], 2.F == value);
                _menus["Window/DisplayScale"]->setItemChecked(_actions["Window/DisplayScale/3.0"], 3.F == value);
            });

        _tooltipsObserver = dtk::ValueObserver<bool>::create(
            app->getWindowModel()->observeTooltips(),
            [this](bool value)
            {
                _menus["Window"]->setItemChecked(_actions["Window/Tooltips"], value);
            });
    }

    void MenuBar::_viewMenuInit(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app)
    {
        _menus["View"] = dtk::Menu::create(context);
        addMenu("View", _menus["View"]);

        _actions["View/ZoomIn"] = std::make_shared<dtk::Action>(
            "Zoom In",
            "ViewZoomIn",
            dtk::Key::Equal,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getViewModel()->zoomIn();
                }
            });
        _actions["View/ZoomIn"]->toolTip = "View zoom in";
        _menus["View"]->addItem(_actions["View/ZoomIn"]);

        _actions["View/ZoomOut"] = std::make_shared<dtk::Action>(
            "Zoom Out",
            "ViewZoomOut",
            dtk::Key::Minus,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getViewModel()->zoomOut();
                }
            });
        _actions["View/ZoomOut"]->toolTip = "View zoom out";
        _menus["View"]->addItem(_actions["View/ZoomOut"]);

        _actions["View/ZoomReset"] = std::make_shared<dtk::Action>(
            "Zoom Reset",
            "ViewZoomReset",
            dtk::Key::_0,
            0,
            [this]
            {
                if (_document)
                {
                    _document->getViewModel()->zoomReset();
                }
            });
        _actions["View/ZoomReset"]->toolTip = "Reset the view zoom";
        _menus["View"]->addItem(_actions["View/ZoomReset"]);

        _menus["View"]->addDivider();

        _actions["View/Frame"] = std::make_shared<dtk::Action>(
            "Frame View",
            "ViewFrame",
            dtk::Key::Backspace,
            0,
            [this](bool value)
            {
                if (_document)
                {
                    _document->getViewModel()->setFrame(value);
                }
            });
        _actions["View/Frame"]->toolTip = "Frame the view";
        _menus["View"]->addItem(_actions["View/Frame"]);
    }

    void MenuBar::_fileMenuUpdate()
    {
        const bool document = _document.get();
        _menus["File"]->setItemEnabled(_actions["File/Close"], document);
        _menus["File"]->setItemEnabled(_actions["File/CloseAll"], document);
        _menus["File"]->setSubMenuEnabled(_menus["Files"], document);
        _menus["File"]->setItemEnabled(_actions["File/Next"], _filesActions.size() > 1);
        _menus["File"]->setItemEnabled(_actions["File/Prev"], _filesActions.size() > 1);
    }

    void MenuBar::_selectMenuUpdate()
    {
        const bool document = _document.get();
        _menus["Select"]->setItemEnabled(_actions["Select/All"], document);
        _menus["Select"]->setItemEnabled(_actions["Select/AllTracks"], document);
        _menus["Select"]->setItemEnabled(_actions["Select/AllClips"], document);
        _menus["Select"]->setItemEnabled(_actions["Select/None"], document);
        _menus["Select"]->setItemEnabled(_actions["Select/Invert"], document);
    }

    void MenuBar::_timeMenuUpdate()
    {
        const bool document = _document.get();
        _menus["Time"]->setItemEnabled(_actions["Time/FrameStart"], document);
        _menus["Time"]->setItemEnabled(_actions["Time/FramePrev"], document);
        _menus["Time"]->setItemEnabled(_actions["Time/FrameNext"], document);
        _menus["Time"]->setItemEnabled(_actions["Time/FrameEnd"], document);
        _menus["Time"]->setItemEnabled(_actions["Time/ClipPrev"], document);
        _menus["Time"]->setItemEnabled(_actions["Time/ClipNext"], document);
    }

    void MenuBar::_playbackMenuUpdate()
    {
        const bool document = _document.get();
        if (document)
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

        _menus["Playback"]->setItemEnabled(_actions["Playback/Stop"], document);
        _menus["Playback"]->setItemEnabled(_actions["Playback/Forward"], document);
        _menus["Playback"]->setItemEnabled(_actions["Playback/Reverse"], document);
        _menus["Playback"]->setItemEnabled(_actions["Playback/Toggle"], document);
    }

    void MenuBar::_windowMenuUpdate()
    {
    }

    void MenuBar::_viewMenuUpdate()
    {
        const bool document = _document.get();
        if (document)
        {
            _frameViewObserver = dtk::ValueObserver<bool>::create(
                _document->getViewModel()->observeFrame(),
                [this](bool value)
                {
                    _menus["View"]->setItemChecked(_actions["View/Frame"], value);
                });
        }
        else
        {
            _frameViewObserver.reset();
        }

        _menus["View"]->setItemEnabled(_actions["View/ZoomIn"], document);
        _menus["View"]->setItemEnabled(_actions["View/ZoomOut"], document);
        _menus["View"]->setItemEnabled(_actions["View/ZoomReset"], document);
        _menus["View"]->setItemEnabled(_actions["View/Frame"], document);
    }
}
