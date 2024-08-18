// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/timeline.h>
#include <opentimelineio/track.h>
#include <opentimelineio/transition.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    //! Create image graphs from a timeline.
    class TimelineGraph : public std::enable_shared_from_this<TimelineGraph>
    {
    public:
        TimelineGraph(
            const std::filesystem::path&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&);

        ~TimelineGraph();

        //! Get the timeline image size.
        const IMATH_NAMESPACE::V2d& getImageSize() const;

        //! Get an image graph for the given time. This function is thread safe.
        std::shared_ptr<IImageNode> exec(const OTIO_NS::RationalTime&) const;

    private:
        std::shared_ptr<IImageNode> _track(
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&) const;
        std::shared_ptr<IImageNode> _item(
            const OTIO_NS::TimeRange& trimmedRangeInParent,
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&) const;

        std::filesystem::path _path;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        IMATH_NAMESPACE::V2i _imageSize = IMATH_NAMESPACE::V2i(0, 0);
    };
}
