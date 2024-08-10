// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageOp.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/timeline.h>
#include <opentimelineio/track.h>

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

        std::shared_ptr<IImageOp> exec(const OTIO_NS::RationalTime&);

    private:
        void _track(
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&);
        void _clip(
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>&);

        std::filesystem::path _path;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        IMATH_NAMESPACE::V2d _size = IMATH_NAMESPACE::V2d(0, 0);
        std::shared_ptr<IImageOp> _op;
    };
}
