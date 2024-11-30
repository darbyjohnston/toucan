// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "PlaybackBar.h"

#include "App.h"
#include "FilesModel.h"

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

        _timeUnitsComboBox = dtk::ComboBox::create(
            context,
            { "Timecode", "Frames", "Seconds" },
            _layout);

        _frameButtons->setCallback(
            [this](TimeAction value)
            {
                if (_file)
                {
                    _file->getPlaybackModel()->timeAction(
                        value,
                        _file->getTimeline());
                }
            });

        _playbackButtons->setCallback(
            [this](Playback value)
            {
                if (_file)
                {
                    _file->getPlaybackModel()->setPlayback(value);
                }
            });

        _timeEdit->setCallback(
            [this](const OTIO_NS::RationalTime& value)
            {
                if (_file)
                {
                    _file->getPlaybackModel()->setCurrentTime(value);
                }
            });

        auto appWeak = std::weak_ptr<App>(app);
        _timeUnitsComboBox->setIndexCallback(
            [appWeak](int value)
            {
                if (auto app = appWeak.lock())
                {
                    app->getTimeUnitsModel()->setTimeUnits(static_cast<TimeUnits>(value));
                }
            });

        _fileObserver = dtk::ValueObserver<std::shared_ptr<File> >::create(
            app->getFilesModel()->observeCurrent(),
            [this](const std::shared_ptr<File>& file)
            {
                _file = file;
                if (file)
                {
                    _timeRangeObserver = dtk::ValueObserver<OTIO_NS::TimeRange>::create(
                        file->getPlaybackModel()->observeTimeRange(),
                        [this](const OTIO_NS::TimeRange& value)
                        {
                            _timeRange = value;
                            _timeRangeUpdate();
                        });

                    _currentTimeObserver = dtk::ValueObserver<OTIO_NS::RationalTime>::create(
                        file->getPlaybackModel()->observeCurrentTime(),
                        [this](const OTIO_NS::RationalTime& value)
                        {
                            _currentTime = value;
                            _currentTimeUpdate();
                        });

                    _playbackObserver = dtk::ValueObserver<Playback>::create(
                        file->getPlaybackModel()->observePlayback(),
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

                _frameButtons->setEnabled(file.get());
                _playbackButtons->setEnabled(file.get());
                _timeEdit->setEnabled(file.get());
                _durationLabel->setEnabled(file.get());
            });

        _timeUnitsObserver = dtk::ValueObserver<TimeUnits>::create(
            app->getTimeUnitsModel()->observeTimeUnits(),
            [this](TimeUnits value)
            {
                _timeUnitsComboBox->setCurrentIndex(static_cast<int>(value));
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
