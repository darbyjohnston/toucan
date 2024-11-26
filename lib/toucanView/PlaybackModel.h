// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <opentimelineio/timeline.h>

#include <dtk/core/ObservableValue.h>
#include <dtk/core/Timer.h>

namespace toucan
{
    //! Time actions.
    enum TimeAction
    {
        FrameStart,
        FramePrev,
        FrameNext,
        FrameEnd,
        ClipNext,
        ClipPrev
    };

    //! Playback actions.
    enum class Playback
    {
        Stop,
        Forward,
        Reverse
    };

    //! Playback model.
    class PlaybackModel : public std::enable_shared_from_this<PlaybackModel>
    {
    public:
        PlaybackModel(const std::shared_ptr<dtk::Context>&);

        virtual ~PlaybackModel();

        //! Get the time range.
        const OTIO_NS::TimeRange& getTimeRange() const;

        //! Observe the time range.
        std::shared_ptr<dtk::IObservableValue<OTIO_NS::TimeRange> > observeTimeRange() const;

        //! Set the time range.
        void setTimeRange(const OTIO_NS::TimeRange&);

        //! Get the current time.
        const OTIO_NS::RationalTime& getCurrentTime() const;

        //! Observe the current time.
        std::shared_ptr<dtk::IObservableValue<OTIO_NS::RationalTime> > observeCurrentTime() const;

        //! Set the current time.
        void setCurrentTime(const OTIO_NS::RationalTime&);

        //! Time actions.
        void timeAction(
            TimeAction,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&);

        //! Get the playback.
        Playback getPlayback() const;

        //! Observe the playback.
        std::shared_ptr<dtk::IObservableValue<Playback> > observePlayback() const;

        //! Set the playback.
        void setPlayback(Playback);

        //! Toggle the playback.
        void togglePlayback();

    private:
        void _timeUpdate();

        std::shared_ptr<dtk::ObservableValue<OTIO_NS::TimeRange> > _timeRange;
        std::shared_ptr<dtk::ObservableValue<OTIO_NS::RationalTime> > _currentTime;
        std::shared_ptr<dtk::ObservableValue<Playback> > _playback;
        Playback _playbackPrev = Playback::Forward;
        std::shared_ptr<dtk::Timer> _timer;
    };
}
