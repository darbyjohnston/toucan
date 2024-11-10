// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "PlaybackBar.h"

#include "App.h"

namespace toucan
{
    void PlaybackBar::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::PlaybackBar", parent);

        _layout = dtk::HorizontalLayout::create(context, shared_from_this());
        _layout->setMarginRole(dtk::SizeRole::MarginInside);
        _layout->setSpacingRole(dtk::SizeRole::SpacingSmall);

        _playbackButtons = PlaybackButtons::create(context, _layout);

        _frameButtons = FrameButtons::create(context, _layout);

        _timeEdit = TimeEdit::create(context, app->getTimeUnitsModel(), _layout);
        _timeEdit->setTooltip("Current time");

        _durationLabel = TimeLabel::create(context, app->getTimeUnitsModel(), _layout);
        _durationLabel->setTooltip("Timeline duration");

        _frameButtons->setCallback(
            [this](FrameAction value)
            {
                if (_document)
                {
                    _document->getPlaybackModel()->frameAction(value);
                }
            });

        _playbackButtons->setCallback(
            [this](Playback value)
            {
                if (_document)
                {
                    _document->getPlaybackModel()->setPlayback(value);
                }
            });

        _timeEdit->setCallback(
            [this](const OTIO_NS::RationalTime& value)
            {
                if (_document)
                {
                    _document->getPlaybackModel()->setCurrentTime(value);
                }
            });

        _documentObserver = dtk::ValueObserver<std::shared_ptr<Document> >::create(
            app->getDocumentsModel()->observeCurrent(),
            [this](const std::shared_ptr<Document>& document)
            {
                _document = document;
                if (document)
                {
                    _timeRangeObserver = dtk::ValueObserver<OTIO_NS::TimeRange>::create(
                        document->getPlaybackModel()->observeTimeRange(),
                        [this](const OTIO_NS::TimeRange& value)
                        {
                            _timeRange = value;
                            _timeRangeUpdate();
                        });

                    _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
                        document->getPlaybackModel()->observeCurrentTime(),
                        [this](const OTIO_NS::RationalTime& value)
                        {
                            _currentTime = value;
                            _currentTimeUpdate();
                        });

                    _playbackObserver = dtk::ValueObserver<Playback>::create(
                        document->getPlaybackModel()->observePlayback(),
                        [this](Playback value)
                        {
                            _playback = value;
                            _playbackUpdate();
                        });
                }
                else
                {
                    _timeRange = OTIO_NS::TimeRange();
                    _currentTime = OTIO_NS::RationalTime();
                    _playback = Playback::Stop;

                    _timeRangeUpdate();
                    _currentTimeUpdate();
                    _playbackUpdate();

                    _timeRangeObserver.reset();
                    _currentTimeObserver.reset();
                    _playbackObserver.reset();
                }

                _frameButtons->setEnabled(document.get());
                _playbackButtons->setEnabled(document.get());
                _timeEdit->setEnabled(document.get());
                _durationLabel->setEnabled(document.get());
            });
    }

        PlaybackBar::~PlaybackBar()
    {}

    std::shared_ptr<PlaybackBar> PlaybackBar::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<PlaybackBar>(new PlaybackBar);
        out->_init(context, app, parent);
        return out;
    }

    void PlaybackBar::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
        _layout->setGeometry(value);
    }

    void PlaybackBar::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        _setSizeHint(_layout->getSizeHint());
    }

    void PlaybackBar::_timelineUpdate()
    {}

    void PlaybackBar::_timeRangeUpdate()
    {
        _timeEdit->setTimeRange(_timeRange);

        _durationLabel->setTime(_timeRange.duration());
    }

    void PlaybackBar::_currentTimeUpdate()
    {
        _timeEdit->setTime(_currentTime);
    }

    void PlaybackBar::_playbackUpdate()
    {
        _playbackButtons->setPlayback(_playback);
    }
}
