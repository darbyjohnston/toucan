// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineTraverse.h"

#include "CompOp.h"
#include "FillOp.h"
#include "LinearTimeWarpOp.h"
#include "ReadOp.h"
#include "SequenceReadOp.h"

#include <opentimelineio/externalReference.h>
#include <opentimelineio/imageSequenceReference.h>
#include <opentimelineio/linearTimeWarp.h>

namespace toucan
{
    TimelineTraverse::TimelineTraverse(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline) :
        _path(path),
        _timeline(timeline)
    {
        for (auto clip : _timeline->find_clips())
        {
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::string url = externalRef->target_url();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<ReadOp>(path);
                const auto buf = read->exec(OTIO_NS::RationalTime(0.0, 1.0));
                const auto& spec = buf.spec();
                if (spec.width > 0)
                {
                    _size.x = spec.width;
                    _size.y = spec.height;
                    break;
                }
            }
            else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(clip->media_reference()))
            {
                const std::string url = sequenceRef->target_url_base();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<SequenceReadOp>(
                    path.string(),
                    sequenceRef->name_prefix(),
                    sequenceRef->name_suffix(),
                    sequenceRef->start_frame(),
                    sequenceRef->frame_step(),
                    sequenceRef->rate(),
                    sequenceRef->frame_zero_padding());
                const auto buf = read->exec(OTIO_NS::RationalTime(0.0, 1.0));
                const auto& spec = buf.spec();
                if (spec.width > 0)
                {
                    _size.x = spec.width;
                    _size.y = spec.height;
                    break;
                }
            }
        }
    }

    TimelineTraverse::~TimelineTraverse()
    {}

    std::shared_ptr<IImageOp> TimelineTraverse::exec(const OTIO_NS::RationalTime& time)
    {
        _op = std::make_shared<FillOp>(FillData{ _size });
        for (const auto& trackIt : _timeline->tracks()->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(trackIt))
            {
                _track(time, track);
            }
        }
        return _op;
    }

    void TimelineTraverse::_track(
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track)
    {
        for (const auto& clipIt : track->children())
        {
            if (auto clip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(clipIt))
            {
                _clip(track->transformed_time(time, clip), clip);
            }
        }
    }

    void TimelineTraverse::_clip(
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip)
    {
        const OTIO_NS::TimeRange trimmedRange = clip->trimmed_range();
        const auto trimmedRangeInParent = clip->trimmed_range_in_parent();
        if (trimmedRange.contains(time) && trimmedRangeInParent.has_value())
        {
            const OTIO_NS::RationalTime timeOffset = trimmedRangeInParent.value().start_time();

            // Get the media reference.
            std::shared_ptr<IImageOp> op;
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::string url = externalRef->target_url();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<ReadOp>(path);
                op = read;
            }
            else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(clip->media_reference()))
            {
                const std::string url = sequenceRef->target_url_base();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<SequenceReadOp>(
                    path.string(),
                    sequenceRef->name_prefix(),
                    sequenceRef->name_suffix(),
                    sequenceRef->start_frame(),
                    sequenceRef->frame_step(),
                    sequenceRef->rate(),
                    sequenceRef->frame_zero_padding());
                op = read;
            }

            // Create the effects.
            for (const auto& effect : clip->effects())
            {
                if (auto iEffect = dynamic_cast<IEffect*>(effect.value))
                {
                    auto effectOp = iEffect->createOp({ op });
                    op = effectOp;
                }
                else if (auto linearTimeWarp = dynamic_cast<OTIO_NS::LinearTimeWarp*>(effect.value))
                {
                    auto linearTimeWarpOp = std::make_shared<LinearTimeWarpOp>(
                        static_cast<float>(linearTimeWarp->time_scalar()),
                        std::vector<std::shared_ptr<IImageOp> >{ op });
                    op = linearTimeWarpOp;
                }
            }
            if (op)
            {
                op->setTimeOffset(timeOffset);
            }

            // Composite.
            auto comp = std::make_shared<CompOp>(
                std::vector<std::shared_ptr<IImageOp> >{ op, _op });
            comp->setPremult(true);
            _op = comp;
        }
    }
}
