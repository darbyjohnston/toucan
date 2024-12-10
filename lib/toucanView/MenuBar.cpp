// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "MenuBar.h"

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
    void MenuBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<MainWindow>& window,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::MenuBar::_init(context, parent);

        _app = app;
        _filesModel = app->getFilesModel();

        _fileMenuInit(context, app);
        _selectMenuInit(context, app);
        _timeMenuInit(context, app);
        _playbackMenuInit(context, app);
        _viewMenuInit(context, app);
        _windowMenuInit(context, app, window);

        _filesObserver = dtk::ListObserver<std::shared_ptr<File> >::create(
            _filesModel->observeFiles(),
            [this](const std::vector<std::shared_ptr<File> >& files)
            {
                _menus["Files"]->clear();
                _filesActions.clear();
                for (int i = 0; i < files.size(); ++i)
                {
                    auto item = std::make_shared<dtk::Action>(
                        files[i]->getPath().filename().string(),
                        [this, i]
                        {
                            _filesModel->setCurrentIndex(i);
                            _menus["File"]->close();
                        });
                    _menus["Files"]->addItem(item);
                    _filesActions.push_back(item);
                }
            });

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            _filesModel->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                _fileMenuUpdate();
                _selectMenuUpdate();
                _timeMenuUpdate();
                _playbackMenuUpdate();
                _viewMenuUpdate();
                _windowMenuUpdate();
            });

        _fileIndexObserver = dtk::ValueObserver<int>::create(
            _filesModel->observeCurrentIndex(),
            [this](int index)
            {
                for (int i = 0; i < _filesActions.size(); ++i)
                {
                    _menus["Files"]->setItemChecked(_filesActions[i], i == index);
                }
            });

        _recentFilesObserver = dtk::ListObserver<std::filesystem::path>::create(
            _filesModel->getRecentFilesModel()->observeRecent(),
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
                            _app.lock()->open(file);
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
                        if (_file)
                        {
                            fileBrowserSystem->setPath(_file->getPath().parent_path());
                        }
                        dtk::FileBrowserOptions options = fileBrowserSystem->getOptions();
                        options.extensions.clear();
                        options.extensions.push_back(".otio");
                        options.extensions.push_back(".otiod");
                        options.extensions.push_back(".otioz");
                        fileBrowserSystem->setOptions(options);
                        fileBrowserSystem->open(
                            getWindow(),
                            [this](const std::filesystem::path& path)
                            {
                                _app.lock()->open(path);
                            },
                            _filesModel->getRecentFilesModel());
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
            [this] { _filesModel->close(); });
        _actions["File/Close"]->toolTip = "Close the current file";
        _menus["File"]->addItem(_actions["File/Close"]);

        _actions["File/CloseAll"] = std::make_shared<dtk::Action>(
            "Close All",
            "FileCloseAll",
            dtk::Key::E,
            static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::KeyModifier::Control),
            [this] { _filesModel->closeAll(); });
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
            [this] { _filesModel->next(); });
        _actions["File/Next"]->toolTip = "Switch to the next file";
        _menus["File"]->addItem(_actions["File/Next"]);

        _actions["File/Prev"] = std::make_shared<dtk::Action>(
            "Previous",
            dtk::Key::PageDown,
            0,
            [this] { _filesModel->prev(); });
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
                if (_file)
                {
                    _file->getSelectionModel()->selectAll(_file->getTimeline());
                }
            });
        _menus["Select"]->addItem(_actions["Select/All"]);

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
        _menus["Select"]->addItem(_actions["Select/AllTracks"]);

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
        _menus["Select"]->addItem(_actions["Select/AllClips"]);

        _actions["Select/None"] = std::make_shared<dtk::Action>(
            "None",
            dtk::Key::A,
            static_cast<int>(dtk::KeyModifier::Shift) |
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->clearSelection();
                }
            });
        _menus["Select"]->addItem(_actions["Select/None"]);

        _actions["Select/Invert"] = std::make_shared<dtk::Action>(
            "Invert",
            dtk::Key::I,
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_file)
                {
                    _file->getSelectionModel()->invertSelection(_file->getTimeline());
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
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FrameStart,
                        _file->getTimeline());
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
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FramePrev,
                        _file->getTimeline());
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
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FrameNext,
                        _file->getTimeline());
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
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FrameEnd,
                        _file->getTimeline());
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
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::ClipNext,
                        _file->getTimeline());
                }
            });
        _menus["Time"]->addItem(_actions["Time/ClipNext"]);

        _actions["Time/ClipPrev"] = std::make_shared<dtk::Action>(
            "Previous Clip",
            dtk::Key::Left,
            static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::ClipPrev,
                        _file->getTimeline());
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
                if (_file)
                {
                    _file->getPlaybackModel()->setInPoint(
                        _file->getPlaybackModel()->getCurrentTime());
                }
            });
        _menus["Time"]->addItem(_actions["Time/InPointSet"]);

        _actions["Time/InPointReset"] = std::make_shared<dtk::Action>(
            "Reset In Point",
            dtk::Key::I,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->resetInPoint();
                }
            });
        _menus["Time"]->addItem(_actions["Time/InPointReset"]);

        _actions["Time/OutPointSet"] = std::make_shared<dtk::Action>(
            "Set Out Point",
            dtk::Key::O,
            0,
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->setOutPoint(
                        _file->getPlaybackModel()->getCurrentTime());
                }
            });
        _menus["Time"]->addItem(_actions["Time/OutPointSet"]);

        _actions["Time/OutPointReset"] = std::make_shared<dtk::Action>(
            "Reset Out Point",
            dtk::Key::O,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->resetOutPoint();
                }
            });
        _menus["Time"]->addItem(_actions["Time/OutPointReset"]);

        _actions["Time/InOutPointReset"] = std::make_shared<dtk::Action>(
            "Reset In/Out Points",
            dtk::Key::P,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->resetInOutPoints();
                }
            });
        _menus["Time"]->addItem(_actions["Time/InOutPointReset"]);

        _actions["Time/InOutPointSelection"] = std::make_shared<dtk::Action>(
            "Set In/Out Points To Selection",
            dtk::Key::P,
            static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::KeyModifier::Control),
            [this]
            {
                if (_file)
                {
                    const auto selection = _file->getSelectionModel()->getSelection();
                    const OTIO_NS::TimeRange& timeRange = _file->getTimelineWrapper()->getTimeRange();
                    const auto timeRangeOpt = getTimeRange(
                        selection,
                        timeRange.start_time(),
                        timeRange.duration().rate());
                    if (timeRangeOpt.has_value())
                    {
                        _file->getPlaybackModel()->setInOutRange(timeRangeOpt.value());
                    }
                    else
                    {
                        _file->getPlaybackModel()->resetInOutPoints();
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
                if (_file)
                {
                    _file->getPlaybackModel()->setPlayback(Playback::Stop);
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
                if (_file)
                {
                    _file->getPlaybackModel()->setPlayback(Playback::Forward);
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
                if (_file)
                {
                    _file->getPlaybackModel()->setPlayback(Playback::Reverse);
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
                if (_file)
                {
                    _file->getPlaybackModel()->togglePlayback();
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
        _menus["View"]->addItem(_actions["View/ZoomIn"]);

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
        _menus["View"]->addItem(_actions["View/ZoomOut"]);

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
        _menus["View"]->addItem(_actions["View/ZoomReset"]);

        _menus["View"]->addDivider();

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
        _menus["View"]->addItem(_actions["View/Frame"]);
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

        struct Component
        {
            WindowComponent component;
            std::string action;
            std::string text;
            std::string icon;
            std::string tooltip;
        };
        const std::vector<Component> components =
        {
            { WindowComponent::ToolBar, "ToolBar", "Tool Bar", "", "" },
            { WindowComponent::ToolsPanel, "ToolsPanel", "Tools Panel", "PanelRight", "Toggle the tools panel" },
            { WindowComponent::PlaybackPanel, "PlaybackPanel", "Playback Panel", "PanelBottom", "Toggle the playback panel" }
        };
        std::weak_ptr<App> appWeak(app);
        for (const auto& component : components)
        {
            const std::string actionName = dtk::Format("Window/{0}").arg(component.action);
            _actions[actionName] = std::make_shared<dtk::Action>(
                component.text,
                component.icon,
                [appWeak, component](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getWindowModel()->setComponent(component.component, value);
                    }
                });
            _actions[actionName]->toolTip = component.tooltip;
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

        _componentsObserver = dtk::MapObserver<WindowComponent, bool>::create(
            app->getWindowModel()->observeComponents(),
            [this](const std::map<WindowComponent, bool>& value)
            {
                auto i = value.find(WindowComponent::ToolBar);
                _menus["Window"]->setItemChecked(_actions["Window/ToolBar"], i->second);
                i = value.find(WindowComponent::ToolsPanel);
                _menus["Window"]->setItemChecked(_actions["Window/ToolsPanel"], i->second);
                i = value.find(WindowComponent::PlaybackPanel);
                _menus["Window"]->setItemChecked(_actions["Window/PlaybackPanel"], i->second);
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

    void MenuBar::_fileMenuUpdate()
    {
        const bool file = _file.get();
        _menus["File"]->setItemEnabled(_actions["File/Close"], file);
        _menus["File"]->setItemEnabled(_actions["File/CloseAll"], file);
        _menus["File"]->setSubMenuEnabled(_menus["Files"], file);
        _menus["File"]->setItemEnabled(_actions["File/Next"], _filesActions.size() > 1);
        _menus["File"]->setItemEnabled(_actions["File/Prev"], _filesActions.size() > 1);
    }

    void MenuBar::_selectMenuUpdate()
    {
        const bool file = _file.get();
        _menus["Select"]->setItemEnabled(_actions["Select/All"], file);
        _menus["Select"]->setItemEnabled(_actions["Select/AllTracks"], file);
        _menus["Select"]->setItemEnabled(_actions["Select/AllClips"], file);
        _menus["Select"]->setItemEnabled(_actions["Select/None"], file);
        _menus["Select"]->setItemEnabled(_actions["Select/Invert"], file);
    }

    void MenuBar::_timeMenuUpdate()
    {
        const bool file = _file.get();
        _menus["Time"]->setItemEnabled(_actions["Time/FrameStart"], file);
        _menus["Time"]->setItemEnabled(_actions["Time/FramePrev"], file);
        _menus["Time"]->setItemEnabled(_actions["Time/FrameNext"], file);
        _menus["Time"]->setItemEnabled(_actions["Time/FrameEnd"], file);
        _menus["Time"]->setItemEnabled(_actions["Time/ClipPrev"], file);
        _menus["Time"]->setItemEnabled(_actions["Time/ClipNext"], file);
    }

    void MenuBar::_playbackMenuUpdate()
    {
        const bool file = _file.get();
        if (file)
        {
            _playbackObserver = dtk::ValueObserver<Playback>::create(
                _file->getPlaybackModel()->observePlayback(),
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

        _menus["Playback"]->setItemEnabled(_actions["Playback/Stop"], file);
        _menus["Playback"]->setItemEnabled(_actions["Playback/Forward"], file);
        _menus["Playback"]->setItemEnabled(_actions["Playback/Reverse"], file);
        _menus["Playback"]->setItemEnabled(_actions["Playback/Toggle"], file);
    }

    void MenuBar::_viewMenuUpdate()
    {
        const bool file = _file.get();
        if (file)
        {
            _frameViewObserver = dtk::ValueObserver<bool>::create(
                _file->getViewModel()->observeFrameView(),
                [this](bool value)
                {
                    _menus["View"]->setItemChecked(_actions["View/Frame"], value);
                });
        }
        else
        {
            _frameViewObserver.reset();
        }

        _menus["View"]->setItemEnabled(_actions["View/ZoomIn"], file);
        _menus["View"]->setItemEnabled(_actions["View/ZoomOut"], file);
        _menus["View"]->setItemEnabled(_actions["View/ZoomReset"], file);
        _menus["View"]->setItemEnabled(_actions["View/Frame"], file);
    }

    void MenuBar::_windowMenuUpdate()
    {
    }
}
