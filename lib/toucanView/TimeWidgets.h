// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "PlaybackModel.h"
#include "TimeUnitsModel.h"

#include <dtk/ui/ButtonGroup.h>
#include <dtk/ui/IncButtons.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/LineEdit.h>
#include <dtk/ui/RowLayout.h>

#include <opentimelineio/anyVector.h>

namespace toucan
{
    class FrameButtons : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~FrameButtons();

        static std::shared_ptr<FrameButtons> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setCallback(const std::function<void(TimeAction)>&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        std::function<void(TimeAction)> _callback;
        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::shared_ptr<dtk::ButtonGroup> _buttonGroup;
    };

    class PlaybackButtons : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~PlaybackButtons();

        static std::shared_ptr<PlaybackButtons> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setPlayback(Playback);

        void setCallback(const std::function<void(Playback)>&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _playbackUpdate();

        Playback _playback = Playback::Stop;
        std::function<void(Playback)> _callback;
        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::shared_ptr<dtk::ButtonGroup> _buttonGroup;
    };

    class TimeEdit : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<TimeUnitsModel>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimeEdit();

        static std::shared_ptr<TimeEdit> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setTime(const OTIO_NS::RationalTime&);
        void setTimeRange(const OTIO_NS::TimeRange&);

        void setCallback(const std::function<void(const OTIO_NS::RationalTime&)>&);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;
        void keyPressEvent(dtk::KeyEvent&) override;
        void keyReleaseEvent(dtk::KeyEvent&) override;

    private:
        void _timeInc(int = 1);
        void _timeDec(int = 1);
        void _timeUpdate();

        OTIO_NS::RationalTime _time;
        OTIO_NS::TimeRange _timeRange;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;

        std::shared_ptr<dtk::HorizontalLayout> _layout;
        std::shared_ptr<dtk::LineEdit> _lineEdit;
        std::shared_ptr<dtk::IncButtons> _incButtons;
        std::function<void(const OTIO_NS::RationalTime&)> _callback;

        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
    };

    class TimeLabel : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<TimeUnitsModel>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimeLabel();

        static std::shared_ptr<TimeLabel> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setTime(const OTIO_NS::RationalTime&);

        void setMarginRole(dtk::SizeRole);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _timeUpdate();

        OTIO_NS::RationalTime _time;
        std::shared_ptr<TimeUnitsModel> _timeUnitsModel;

        std::shared_ptr<dtk::Label> _label;

        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
    };
}

