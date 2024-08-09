// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineTraverse.h"

#include "ImageComp.h"
#include "ImageRead.h"

#include <opentimelineio/externalReference.h>

namespace toucan
{
    TimelineTraverse::TimelineTraverse(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline) :
        _path(path),
        _timeline(timeline)
    {}

    TimelineTraverse::~TimelineTraverse()
    {}

    std::shared_ptr<IImageOp> TimelineTraverse::exec(const OTIO_NS::RationalTime& time)
    {
        //std::cout << "traverse: " << time.value() << std::endl;
        _comp.reset();
        for (const auto& trackIt : _timeline->tracks()->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(trackIt))
            {
                _track(time, track);
            }
        }
        return _comp;
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
        const auto availableRange = clip->available_range();
        if (availableRange.contains(time))
        {
            //std::cout << "  clip " << clip->name() << ": " << time.value() << std::endl;
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::string url = externalRef->target_url();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<ImageRead>();
                read->setPath(path);

                auto comp = std::make_shared<ImageComp>();
                comp->setPremult(true);
                if (_comp)
                {
                    comp->setInputs({ read, _comp });
                }
                else
                {
                    comp->setInputs({ read });
                }
                _comp = comp;
            }
        }
    }
}
