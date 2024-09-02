// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include "App.h"

using namespace dtk;
using namespace dtk::core;
using namespace dtk::ui;

namespace toucan
{
    void TimelineWidget::_init(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, "toucan::TimelineWidget", parent);

        _layout = VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(SizeRole::None);

        auto hLayout = HorizontalLayout::create(context, _layout);
        hLayout->setMarginRole(SizeRole::MarginInside);
        hLayout->setSpacingRole(SizeRole::SpacingSmall);

        _playbackButtons = PlaybackButtons::create(context, hLayout);
        _frameButtons = FrameButtons::create(context, hLayout);
        _timeEdit = TimeEdit::create(context, hLayout);
        _slider = IntSlider::create(context, nullptr, hLayout);
        _durationLabel = TimeLabel::create(context, hLayout);

        auto weakApp = std::weak_ptr<App>(app);
        _frameButtons->setCallback(
            [weakApp](FrameAction value)
            {
                if (auto app = weakApp.lock())
                {
                    app->getPlaybackModel()->frameAction(value);
                }
            });

        _playbackButtons->setCallback(
            [weakApp](Playback value)
            {
                if (auto app = weakApp.lock())
                {
                    app->getPlaybackModel()->setPlayback(value);
                }
            });

        _timeEdit->setCallback(
            [weakApp, this](const OTIO_NS::RationalTime& value)
            {
                if (auto app = weakApp.lock())
                {
                    app->getPlaybackModel()->setCurrentTime(value);
                }
            });

        _slider->setCallback(
            [weakApp, this](double value)
            {
                if (auto app = weakApp.lock())
                {
                    app->getPlaybackModel()->setCurrentTime(OTIO_NS::RationalTime(
                        value,
                        _timeRange.duration().rate()));
                }
            });

        _timeRangeObserver = ValueObserver<OTIO_NS::TimeRange>::create(
            app->getPlaybackModel()->observeTimeRange(),
            [this](const OTIO_NS::TimeRange& value)
            {
                _timeRange = value;
                _timeRangeUpdate();
            });

        _currentTimeObserver = ValueObserver<OTIO_NS::RationalTime>::create(
            app->getPlaybackModel()->observeCurrentTime(),
            [this](const OTIO_NS::RationalTime& value)
            {
                _currentTime = value;
                _currentTimeUpdate();
            });

        _playbackObserver = ValueObserver<Playback>::create(
            app->getPlaybackModel()->observePlayback(),
            [this](Playback value)
            {
                _playback = value;
                _playbackUpdate();
            });
    }

    TimelineWidget::~TimelineWidget()
    {}

    std::shared_ptr<TimelineWidget> TimelineWidget::create(
        const std::shared_ptr<Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::shared_ptr<TimelineWidget>(new TimelineWidget);
        out->_init(context, app, parent);
        return out;
    }

    void TimelineWidget::setGeometry(const Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void TimelineWidget::sizeHintEvent(const SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void TimelineWidget::_timelineUpdate()
    {}

    void TimelineWidget::_timeRangeUpdate()
    {
        _timeEdit->setTimeRange(_timeRange);

        _slider->setRange(RangeI(
            _timeRange.start_time().value(),
            _timeRange.end_time_inclusive().value()));

        _durationLabel->setTime(_timeRange.duration());
    }

    void TimelineWidget::_currentTimeUpdate()
    {
        _timeEdit->setTime(_currentTime);

        _slider->setValue(_currentTime.value());
    }

    void TimelineWidget::_playbackUpdate()
    {
        _playbackButtons->setPlayback(_playback);
    }
}
