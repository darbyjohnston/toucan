// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "PlaybackModel.h"

namespace toucan
{
    PlaybackModel::PlaybackModel(const std::shared_ptr<dtk::Context>& context)
    {
        _timeRange = dtk::ObservableValue<OTIO_NS::TimeRange>::create();
        _currentTime = dtk::ObservableValue<OTIO_NS::RationalTime>::create(OTIO_NS::RationalTime(-1.0, -1.0));
        _playback = dtk::ObservableValue<Playback>::create(Playback::Stop);
        _timer = dtk::Timer::create(context);
        _timer->setRepeating(true);
    }

    PlaybackModel::~PlaybackModel()
    {}

    const OTIO_NS::TimeRange& PlaybackModel::getTimeRange() const
    {
        return _timeRange->get();
    }

    std::shared_ptr<dtk::IObservableValue<OTIO_NS::TimeRange> > PlaybackModel::observeTimeRange() const
    {
        return _timeRange;
    }

    void PlaybackModel::setTimeRange(const OTIO_NS::TimeRange& value)
    {
        if (_timeRange->setIfChanged(value))
        {
            _currentTime->setIfChanged(value.start_time());
        }
    }

    const OTIO_NS::RationalTime& PlaybackModel::getCurrentTime() const
    {
        return _currentTime->get();
    }

    std::shared_ptr<dtk::IObservableValue<OTIO_NS::RationalTime> > PlaybackModel::observeCurrentTime() const
    {
        return _currentTime;
    }

    void PlaybackModel::setCurrentTime(const OTIO_NS::RationalTime& value)
    {
        OTIO_NS::RationalTime time = value;
        if (!time.is_invalid_time())
        {
            const OTIO_NS::TimeRange& range = _timeRange->get();
            if (time > range.end_time_inclusive())
            {
                time = range.start_time();
            }
            else if (time < range.start_time())
            {
                time = range.end_time_exclusive();
            }
        }
        if (time != _currentTime->get())
        {
            _playback->setIfChanged(Playback::Stop);
            _currentTime->setIfChanged(time);
        }
    }

    void PlaybackModel::frameAction(FrameAction value)
    {
        const OTIO_NS::TimeRange& timeRange = _timeRange->get();
        const OTIO_NS::RationalTime& time = _currentTime->get();
        switch (value)
        {
        case FrameAction::Start:
            setCurrentTime(timeRange.start_time());
            break;
        case FrameAction::Prev:
            setCurrentTime(time - OTIO_NS::RationalTime(1.0, time.rate()));
            break;
        case FrameAction::Next:
            setCurrentTime(time + OTIO_NS::RationalTime(1.0, time.rate()));
            break;
        case FrameAction::End:
            setCurrentTime(timeRange.end_time_inclusive());
            break;
        default: break;
        }
    }

    Playback PlaybackModel::getPlayback() const
    {
        return _playback->get();
    }

    std::shared_ptr<dtk::IObservableValue<Playback> > PlaybackModel::observePlayback() const
    {
        return _playback;
    }

    void PlaybackModel::setPlayback(Playback value)
    {
        const Playback prev = _playback->get();
        if (_playback->setIfChanged(value))
        {
            switch (value)
            {
            case Playback::Stop:
                _timer->stop();
                _playbackPrev = prev;
                break;
            case Playback::Forward:
            case Playback::Reverse:
                _timer->start(
                    std::chrono::microseconds(static_cast<int>(1000 / _currentTime->get().rate())),
                    [this]
                    {
                        _timeUpdate();
                    });
                break;
            default: break;
            }
        }
    }

    void PlaybackModel::togglePlayback()
    {
        setPlayback(
            Playback::Stop == _playback->get() ?
            _playbackPrev :
            Playback::Stop);
    }

    void PlaybackModel::_timeUpdate()
    {
        switch (_playback->get())
        {
        case Playback::Forward:
        {
            auto time = _currentTime->get() + OTIO_NS::RationalTime(1.0, _currentTime->get().rate());
            if (time > _timeRange->get().end_time_inclusive())
            {
                time = _timeRange->get().start_time();
            }
            _currentTime->setIfChanged(time);
            break;
        }
        case Playback::Reverse:
        {
            auto time = _currentTime->get() - OTIO_NS::RationalTime(1.0, _currentTime->get().rate());
            if (time < _timeRange->get().start_time())
            {
                time = _timeRange->get().end_time_inclusive();
            }
            _currentTime->setIfChanged(time);
            break;
        }
        default: break;
        }
    }
}
