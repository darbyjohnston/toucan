// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Timeline.h"

#include "Clip.h"
#include "Gap.h"
#include "MediaReference.h"
#include "Stack.h"
#include "Track.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/externalReference.h>
#include <opentimelineio/imageSequenceReference.h>
#include <opentimelineio/gap.h>

namespace toucan
{
    OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> convertTo(
        const std::shared_ptr<Timeline>& timeline)
    {
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> out;
        return out;
    }

    namespace
    {
        std::shared_ptr<Clip> convertFrom(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& otioClip)
        {
            auto clip = std::make_shared<Clip>();
            clip->setName(otioClip->name());
            clip->setRange(otioClip->trimmed_range());

            Clip::MediaReferences refs;
            for (const auto& otioRef : otioClip->media_references())
            {
                if (auto otioExternalRef = dynamic_cast<OTIO_NS::ExternalReference*>(otioRef.second))
                {
                    auto ref = std::make_shared<ExternalReference>();
                    ref->setURL(otioExternalRef->target_url());
                    refs[otioRef.first] = ref;
                }
            }
            clip->setMediaReferences(refs);
            clip->setActiveMediaReference(otioClip->active_media_reference_key());

            return clip;
        }

        std::shared_ptr<Gap> convertFrom(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap>& otioGap)
        {
            auto gap = std::make_shared<Gap>();
            gap->setName(otioGap->name());
            gap->setRange(otioGap->trimmed_range());
            return gap;
        }

        std::shared_ptr<Track> convertFrom(
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& otioTrack)
        {
            auto track = std::make_shared<Track>(otioTrack->kind());
            track->setName(otioTrack->name());
            track->setRange(otioTrack->trimmed_range());
            for (const auto& otioItem : otioTrack->children())
            {
                if (auto otioClip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(otioItem))
                {
                    auto clip = convertFrom(otioClip);
                    track->addChild(clip);
                }
                else if (auto otioGap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Gap>(otioItem))
                {
                    auto gap = convertFrom(otioGap);
                    track->addChild(gap);
                }
            }
            return track;
        }
    }

    std::shared_ptr<Timeline> convertFrom(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& otioTimeline)
    {
        auto out = std::make_shared<Timeline>(path);

        const OTIO_NS::RationalTime& duration = otioTimeline->duration();
        OTIO_NS::RationalTime startTime(0.0, duration.rate());
        auto opt = otioTimeline->global_start_time();
        if (opt.has_value())
        {
            startTime = opt.value();
        }
        out->setRange(OTIO_NS::TimeRange(startTime, duration));

        out->getStack()->setRange(otioTimeline->tracks()->trimmed_range());

        for (const auto& otioItem : otioTimeline->tracks()->children())
        {
            if (auto otioTrack = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(otioItem))
            {
                auto track = convertFrom(otioTrack);
                out->getStack()->addChild(track);
            }
        }

        return out;
    }
}
