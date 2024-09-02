// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "TimeWidget.h"

#include <dtk/ui/IntSlider.h>
#include <dtk/ui/RowLayout.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    class App;

    class TimelineWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~TimelineWidget();

        static std::shared_ptr<TimelineWidget> create(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

        void setGeometry(const dtk::Box2I&) override;
        void sizeHintEvent(const dtk::SizeHintEvent&) override;

    private:
        void _timelineUpdate();
        void _timeRangeUpdate();
        void _currentTimeUpdate();
        void _playbackUpdate();

        std::shared_ptr<dtk::ValueObserver<OTIO_NS::TimeRange> > _timeRangeObserver;
        OTIO_NS::TimeRange _timeRange;
        std::shared_ptr<dtk::ValueObserver<OTIO_NS::RationalTime> > _currentTimeObserver;
        OTIO_NS::RationalTime _currentTime;
        std::shared_ptr<dtk::ValueObserver<Playback> > _playbackObserver;
        Playback _playback = Playback::Stop;

        std::shared_ptr<dtk::VerticalLayout> _layout;
        std::shared_ptr<FrameButtons> _frameButtons;
        std::shared_ptr<PlaybackButtons> _playbackButtons;
        std::shared_ptr<TimeEdit> _timeEdit;
        std::shared_ptr<dtk::IntSlider> _slider;
        std::shared_ptr<TimeLabel> _durationLabel;
    };
}
