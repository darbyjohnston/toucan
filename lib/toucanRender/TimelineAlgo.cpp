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
}
