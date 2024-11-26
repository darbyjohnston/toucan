// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "PlaybackModel.h"

#include <toucan/TimelineAlgo.h>

#include <opentimelineio/clip.h>

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
                time = range.end_time_inclusive();
            }
        }
        if (time != _currentTime->get())
        {
            _playback->setIfChanged(Playback::Stop);
            _currentTime->setIfChanged(time);
        }
    }

    void PlaybackModel::timeAction(
        TimeAction value,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline)
    {
        const OTIO_NS::TimeRange& timeRange = _timeRange->get();
        const OTIO_NS::RationalTime& currentTime = _currentTime->get();
        switch (value)
        {
        case TimeAction::FrameStart:
            setCurrentTime(timeRange.start_time());
            break;
        case TimeAction::FramePrev:
            setCurrentTime(currentTime - OTIO_NS::RationalTime(1.0, currentTime.rate()));
            break;
        case TimeAction::FrameNext:
            setCurrentTime(currentTime + OTIO_NS::RationalTime(1.0, currentTime.rate()));
            break;
        case TimeAction::FrameEnd:
            setCurrentTime(timeRange.end_time_inclusive());
            break;
        case TimeAction::ClipNext:
        {
            std::optional<OTIO_NS::RationalTime> min;
            std::optional<OTIO_NS::RationalTime> diff;
            std::optional<OTIO_NS::RationalTime> t;
            const auto clips = getVideoClips(timeline);
            auto i = clips.begin();
            if (i != clips.end())
            {
                const auto clipRangeOpt = (*i)->trimmed_range_in_parent();
                if (clipRangeOpt.has_value())
                {
                    const OTIO_NS::RationalTime start =
                        clipRangeOpt.value().start_time() +
                        timeRange.start_time();
                    min = start;
                    if (start > currentTime)
                    {
                        diff = currentTime - start;
                        diff = OTIO_NS::RationalTime(std::abs(diff.value().value()), diff.value().rate());
                        t = start;
                    }
                }
                ++i;
            }
            for (; i != clips.end(); ++i)
            {
                const auto clipRangeOpt = (*i)->trimmed_range_in_parent();
                if (clipRangeOpt.has_value())
                {
                    const OTIO_NS::RationalTime start =
                        clipRangeOpt.value().start_time() +
                        timeRange.start_time();
                    min = min.has_value() ? std::min(min.value(), start) : start;
                    if (start > currentTime)
                    {
                        OTIO_NS::RationalTime clipDiff = currentTime - start;
                        clipDiff = OTIO_NS::RationalTime(std::abs(clipDiff.value()), clipDiff.rate());
                        if (!diff.has_value() || clipDiff < diff)
                        {
                            diff = clipDiff;
                            t = start;
                        }
                    }
                }
            }
            if (t.has_value())
            {
                setCurrentTime(t.value());
            }
            else if (min.has_value())
            {
                setCurrentTime(min.value());
            }
            break;
        }
        case TimeAction::ClipPrev:
        {
            std::optional<OTIO_NS::RationalTime> max;
            std::optional<OTIO_NS::RationalTime> diff;
            std::optional<OTIO_NS::RationalTime> t;
            const auto clips = getVideoClips(timeline);
            auto i = clips.begin();
            if (i != clips.end())
            {
                const auto clipRangeOpt = (*i)->trimmed_range_in_parent();
                if (clipRangeOpt.has_value())
                {
                    const OTIO_NS::RationalTime start =
                        clipRangeOpt.value().start_time() +
                        timeRange.start_time();
                    max = start;
                    if (start < currentTime)
                    {
                        diff = currentTime - start;
                        diff = OTIO_NS::RationalTime(std::abs(diff.value().value()), diff.value().rate());
                        t = start;
                    }
                }
                ++i;
            }
            for (; i != clips.end(); ++i)
            {
                const auto clipRangeOpt = (*i)->trimmed_range_in_parent();
                if (clipRangeOpt.has_value())
                {
                    const OTIO_NS::RationalTime start =
                        clipRangeOpt.value().start_time() +
                        timeRange.start_time();
                    max = max.has_value() ? std::max(max.value(), start) : start;
                    if (start < currentTime)
                    {
                        OTIO_NS::RationalTime clipDiff = currentTime - start;
                        clipDiff = OTIO_NS::RationalTime(std::abs(clipDiff.value()), clipDiff.rate());
                        if (!diff.has_value() || clipDiff < diff)
                        {
                            diff = clipDiff;
                            t = start;
                        }
                    }
                }
            }
            if (t.has_value())
            {
                setCurrentTime(t.value());
            }
            else if (max.has_value())
            {
                setCurrentTime(max.value());
            }
            break;
        }
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
