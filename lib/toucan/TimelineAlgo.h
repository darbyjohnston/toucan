// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <opentimelineio/timeline.h>

namespace toucan
{
    //! Get the video clips in a timeline.
    std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip> >
        getVideoClips(const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&);

    //! Get the range of multiple items.
    std::optional<OTIO_NS::TimeRange> getTimeRange(
        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> >&,
        const OTIO_NS::RationalTime& startTime,
        double rate);
}
