// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include "Type.h"

#include <opentimelineio/timeline.h>

#include <dtkCore/ObservableValue.h>
#include <dtkCore/Timer.h>

namespace toucan
{
    class PlaybackModel : public std::enable_shared_from_this<PlaybackModel>
    {
    public:
        PlaybackModel(const std::shared_ptr<dtk::core::Context>&);

        virtual ~PlaybackModel();

        const OTIO_NS::TimeRange& getTimeRange() const;

        std::shared_ptr<dtk::core::IObservableValue<OTIO_NS::TimeRange> > observeTimeRange() const;

        void setTimeRange(const OTIO_NS::TimeRange&);

        const OTIO_NS::RationalTime& getCurrentTime() const;

        std::shared_ptr<dtk::core::IObservableValue<OTIO_NS::RationalTime> > observeCurrentTime() const;

        void setCurrentTime(const OTIO_NS::RationalTime&);

        void frameAction(FrameAction);

        std::shared_ptr<dtk::core::IObservableValue<Playback> > observePlayback() const;

        void setPlayback(Playback);

    private:
        void _timeUpdate();

        std::weak_ptr<dtk::core::Context> _context;
        std::shared_ptr<dtk::core::ObservableValue<OTIO_NS::TimeRange> > _timeRange;
        std::shared_ptr<dtk::core::ObservableValue<OTIO_NS::RationalTime> > _currentTime;
        std::shared_ptr<dtk::core::ObservableValue<Playback> > _playback;
        std::shared_ptr<dtk::core::Timer> _timer;
    };
}
