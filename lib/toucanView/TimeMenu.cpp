// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimeMenu.h"

#include "App.h"
#include "FilesModel.h"
#include "PlaybackModel.h"
#include "SelectionModel.h"

#include <dtk/ui/Action.h>

namespace toucan
{
    void TimeMenu::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::Menu::_init(context, parent);

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
        addItem(_actions["Time/FrameStart"]);

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
        addItem(_actions["Time/FramePrev"]);

        _actions["Time/FramePrevX10"] = std::make_shared<dtk::Action>(
            "Previous Frame X10",
            dtk::Key::Left,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FramePrevX10,
                        _file->getTimeline());
                }
            });
        addItem(_actions["Time/FramePrevX10"]);

        _actions["Time/FramePrevX100"] = std::make_shared<dtk::Action>(
            "Previous Frame X100",
            dtk::Key::Left,
            static_cast<int>(dtk::commandKeyModifier),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FramePrevX100,
                        _file->getTimeline());
                }
            });
        addItem(_actions["Time/FramePrevX100"]);

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
        addItem(_actions["Time/FrameNext"]);

        _actions["Time/FrameNextX10"] = std::make_shared<dtk::Action>(
            "Next Frame X10",
            dtk::Key::Right,
            static_cast<int>(dtk::KeyModifier::Shift),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FrameNextX10,
                        _file->getTimeline());
                }
            });
        addItem(_actions["Time/FrameNextX10"]);

        _actions["Time/FrameNextX100"] = std::make_shared<dtk::Action>(
            "Next Frame X100",
            dtk::Key::Right,
            static_cast<int>(dtk::commandKeyModifier),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::FrameNextX100,
                        _file->getTimeline());
                }
            });
        addItem(_actions["Time/FrameNextX100"]);

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
        addItem(_actions["Time/FrameEnd"]);

        addDivider();

        _actions["Time/ClipNext"] = std::make_shared<dtk::Action>(
            "Next Clip",
            dtk::Key::Right,
            static_cast<int>(dtk::KeyModifier::Alt),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::ClipNext,
                        _file->getTimeline());
                }
            });
        addItem(_actions["Time/ClipNext"]);

        _actions["Time/ClipPrev"] = std::make_shared<dtk::Action>(
            "Previous Clip",
            dtk::Key::Left,
            static_cast<int>(dtk::KeyModifier::Alt),
            [this]
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        TimeAction::ClipPrev,
                        _file->getTimeline());
                }
            });
        addItem(_actions["Time/ClipPrev"]);

        addDivider();

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
        addItem(_actions["Time/InPointSet"]);

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
        addItem(_actions["Time/InPointReset"]);

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
        addItem(_actions["Time/OutPointSet"]);

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
        addItem(_actions["Time/OutPointReset"]);

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
        addItem(_actions["Time/InOutPointReset"]);

        _actions["Time/InOutPointSelection"] = std::make_shared<dtk::Action>(
            "Set In/Out Points To Selection",
            dtk::Key::P,
            static_cast<int>(dtk::KeyModifier::Shift) | static_cast<int>(dtk::commandKeyModifier),
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
                        _file->getPlaybackModel()->setCurrentTime(timeRangeOpt.value().start_time());
                    }
                    else
                    {
                        _file->getPlaybackModel()->resetInOutPoints();
                    }
                }
            });
        addItem(_actions["Time/InOutPointSelection"]);

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                _menuUpdate();
            });
    }

    TimeMenu::~TimeMenu()
    {}

    std::shared_ptr<TimeMenu> TimeMenu::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<TimeMenu>(new TimeMenu);
        out->_init(context, app, parent);
        return out;
    }

    const std::map<std::string, std::shared_ptr<dtk::Action> >& TimeMenu::getActions() const
    {
        return _actions;
    }

    void TimeMenu::_menuUpdate()
    {
        const bool file = _file.get();
        setItemEnabled(_actions["Time/FrameStart"], file);
        setItemEnabled(_actions["Time/FramePrev"], file);
        setItemEnabled(_actions["Time/FramePrevX10"], file);
        setItemEnabled(_actions["Time/FramePrevX100"], file);
        setItemEnabled(_actions["Time/FrameNext"], file);
        setItemEnabled(_actions["Time/FrameNextX10"], file);
        setItemEnabled(_actions["Time/FrameNextX100"], file);
        setItemEnabled(_actions["Time/FrameEnd"], file);
        setItemEnabled(_actions["Time/ClipPrev"], file);
        setItemEnabled(_actions["Time/ClipNext"], file);
    }
}
