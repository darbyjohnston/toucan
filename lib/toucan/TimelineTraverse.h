// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/timeline.h>
#include <opentimelineio/track.h>
#include <opentimelineio/transition.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    //! Traverse a timeline.
    class TimelineTraverse : public std::enable_shared_from_this<TimelineTraverse>
    {
    public:
        TimelineTraverse(
            const std::filesystem::path&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&);

        ~TimelineTraverse();

        //! Get the timeline image size.
        const IMATH_NAMESPACE::V2d& getImageSize() const;

        //! Get an image operation graph for the given time. This function is
        //! thread safe.
        std::shared_ptr<IImageOp> exec(const OTIO_NS::RationalTime&) const;

    private:
        std::shared_ptr<IImageOp> _track(
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&) const;
        std::shared_ptr<IImageOp> _item(
            const OTIO_NS::TimeRange& trimmedRangeInParent,
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&) const;

        std::filesystem::path _path;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        IMATH_NAMESPACE::V2d _imageSize = IMATH_NAMESPACE::V2d(0, 0);
    };
}
