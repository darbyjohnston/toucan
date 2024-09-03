// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <opentimelineio/timeline.h>

#include <dtk/core/ObservableValue.h>
#include <dtk/core/Timer.h>

namespace toucan
{
    enum FrameAction
    {
        Start,
        Prev,
        Next,
        End
    };

    enum class Playback
    {
        Stop,
        Forward,
        Reverse
    };

    class PlaybackModel : public std::enable_shared_from_this<PlaybackModel>
    {
    public:
        PlaybackModel(const std::shared_ptr<dtk::Context>&);

        virtual ~PlaybackModel();

        const OTIO_NS::TimeRange& getTimeRange() const;

        std::shared_ptr<dtk::IObservableValue<OTIO_NS::TimeRange> > observeTimeRange() const;

        void setTimeRange(const OTIO_NS::TimeRange&);

        const OTIO_NS::RationalTime& getCurrentTime() const;

        std::shared_ptr<dtk::IObservableValue<OTIO_NS::RationalTime> > observeCurrentTime() const;

        void setCurrentTime(const OTIO_NS::RationalTime&);

        void frameAction(FrameAction);

        std::shared_ptr<dtk::IObservableValue<Playback> > observePlayback() const;

        void setPlayback(Playback);

        void togglePlayback();

    private:
        void _timeUpdate();

        std::weak_ptr<dtk::Context> _context;
        std::shared_ptr<dtk::ObservableValue<OTIO_NS::TimeRange> > _timeRange;
        std::shared_ptr<dtk::ObservableValue<OTIO_NS::RationalTime> > _currentTime;
        std::shared_ptr<dtk::ObservableValue<Playback> > _playback;
        Playback _playbackPrev = Playback::Forward;
        std::shared_ptr<dtk::Timer> _timer;
    };
}
