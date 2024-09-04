// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "BottomBar.h"

#include "App.h"

namespace toucan
{
    void BottomBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::BottomBar", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _playbackButtons = PlaybackButtons::create(context, _layout);

        _frameButtons = FrameButtons::create(context, _layout);

        _timeEdit = TimeEdit::create(context, _layout);
        _timeEdit->setTooltip("Current time");

        _slider = dtk::IntSlider::create(context, nullptr, _layout);

        _durationLabel = TimeLabel::create(context, _layout);
        _durationLabel->setTooltip("Timeline duration");

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

        _timeRangeObserver = dtk::ValueObserver<OTIO_NS::TimeRange>::create(
            app->getPlaybackModel()->observeTimeRange(),
            [this](const OTIO_NS::TimeRange& value)
            {
                _timeRange = value;
                _timeRangeUpdate();
            });

        _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
            app->getPlaybackModel()->observeCurrentTime(),
            [this](const OTIO_NS::RationalTime& value)
            {
                _currentTime = value;
                _currentTimeUpdate();
            });

        _playbackObserver = dtk::ValueObserver<Playback>::create(
            app->getPlaybackModel()->observePlayback(),
            [this](Playback value)
            {
                _playback = value;
                _playbackUpdate();
            });
    }

    BottomBar::~BottomBar()
    {}

    std::shared_ptr<BottomBar> BottomBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<BottomBar>(new BottomBar);
        out->_init(context, app, parent);
        return out;
    }

    void BottomBar::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void BottomBar::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void BottomBar::_timelineUpdate()
    {}

    void BottomBar::_timeRangeUpdate()
    {
        _timeEdit->setTimeRange(_timeRange);

        _slider->setRange(dtk::RangeI(
            _timeRange.start_time().value(),
            _timeRange.end_time_inclusive().value()));

        _durationLabel->setTime(_timeRange.duration());
    }

    void BottomBar::_currentTimeUpdate()
    {
        _timeEdit->setTime(_currentTime);

        _slider->setValue(_currentTime.value());
    }

    void BottomBar::_playbackUpdate()
    {
        _playbackButtons->setPlayback(_playback);
    }
}
