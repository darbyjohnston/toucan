// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Type.h"

#include <dtkUI/ButtonGroup.h>
#include <dtkUI/IncButtons.h>
#include <dtkUI/Label.h>
#include <dtkUI/LineEdit.h>
#include <dtkUI/RowLayout.h>

#include <opentimelineio/anyVector.h>

namespace toucan
{
    class FrameButtons : public dtk::ui::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~FrameButtons();

        static std::shared_ptr<FrameButtons> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setCallback(const std::function<void(FrameAction)>&);

        void setGeometry(const dtk::core::Box2I&) override;
        void sizeHintEvent(const dtk::ui::SizeHintEvent&) override;

    private:
        std::function<void(FrameAction)> _callback;
        std::shared_ptr<dtk::ui::HorizontalLayout> _layout;
        std::shared_ptr<dtk::ui::ButtonGroup> _buttonGroup;
    };

    class PlaybackButtons : public dtk::ui::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~PlaybackButtons();

        static std::shared_ptr<PlaybackButtons> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setPlayback(Playback);

        void setCallback(const std::function<void(Playback)>&);

        void setGeometry(const dtk::core::Box2I&) override;
        void sizeHintEvent(const dtk::ui::SizeHintEvent&) override;

    private:
        void _playbackUpdate();

        Playback _playback = Playback::Stop;
        std::function<void(Playback)> _callback;
        std::shared_ptr<dtk::ui::HorizontalLayout> _layout;
        std::shared_ptr<dtk::ui::ButtonGroup> _buttonGroup;
    };

    class TimeEdit : public dtk::ui::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimeEdit();

        static std::shared_ptr<TimeEdit> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setTime(const OTIO_NS::RationalTime&);
        void setTimeRange(const OTIO_NS::TimeRange&);

        void setCallback(const std::function<void(const OTIO_NS::RationalTime&)>&);

        void setGeometry(const dtk::core::Box2I&) override;
        void sizeHintEvent(const dtk::ui::SizeHintEvent&) override;
        void keyPressEvent(dtk::ui::KeyEvent&) override;
        void keyReleaseEvent(dtk::ui::KeyEvent&) override;

    private:
        void _timeInc(int = 1);
        void _timeDec(int = 1);
        void _timeUpdate();

        OTIO_NS::RationalTime _time;
        OTIO_NS::TimeRange _timeRange;
        std::shared_ptr<dtk::ui::HorizontalLayout> _layout;
        std::shared_ptr<dtk::ui::LineEdit> _lineEdit;
        std::shared_ptr<dtk::ui::IncButtons> _incButtons;
        std::function<void(const OTIO_NS::RationalTime&)> _callback;
    };

    class TimeLabel : public dtk::ui::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimeLabel();

        static std::shared_ptr<TimeLabel> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setTime(const OTIO_NS::RationalTime&);

        void setMarginRole(dtk::ui::SizeRole);

        void setGeometry(const dtk::core::Box2I&) override;
        void sizeHintEvent(const dtk::ui::SizeHintEvent&) override;

    private:
        void _timeUpdate();

        OTIO_NS::RationalTime _time;
        std::shared_ptr<dtk::ui::Label> _label;
    };
}

