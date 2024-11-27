// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimelineAlgo.h"

#include <opentimelineio/clip.h>

namespace toucan
{
    std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip> >
        getVideoClips(const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline)
    {
        std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip> > out;
        for (const auto& child : timeline->tracks()->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(child))
            {
                if (OTIO_NS::Track::Kind::video == track->kind())
                {
                    const auto clips = track->find_clips(nullptr, std::nullopt, true);
                    out.insert(out.end(), clips.begin(), clips.end());
                }
            }
        }
        return out;
    }

    std::optional<OTIO_NS::TimeRange> getTimeRange(
        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >& items,
        const OTIO_NS::RationalTime& startTime,
        double rate)
    {
        std::optional<OTIO_NS::TimeRange> out;
        for (const auto& item : items)
        {
            //! \bug Shouldn't trimmed_range_in_parent() check whether the parent
            //! is null?
            if (item->parent())
            {
                const auto timeRangeOpt = item->trimmed_range_in_parent();
                if (timeRangeOpt.has_value())
                {
                    const OTIO_NS::TimeRange timeRange(
                        timeRangeOpt.value().start_time() + startTime,
                        timeRangeOpt.value().duration());
                    if (out.has_value())
                    {
                        out = OTIO_NS::TimeRange::range_from_start_end_time_inclusive(
                            std::min(
                                out.value().start_time(),
                                timeRange.start_time().rescaled_to(rate).round()),
                            std::max(
                                out.value().end_time_inclusive(),
                                timeRange.end_time_inclusive().rescaled_to(rate).round()));
                    }
                    else
                    {
                        out = OTIO_NS::TimeRange(
                            timeRange.start_time().rescaled_to(rate).round(),
                            timeRange.duration().rescaled_to(rate).round());
                    }
                }
            }
        }
        return out;
    }
}
