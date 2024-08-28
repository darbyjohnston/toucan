// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>
#include <toucan/MessageLog.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/timeline.h>
#include <opentimelineio/track.h>
#include <opentimelineio/transition.h>

#include <filesystem>
#include <memory>

namespace toucan
{
    //! Image graph options.
    struct ImageGraphOptions
    {
        std::shared_ptr<MessageLog> log;
    };

    //! Create image graphs from a timeline.
    class ImageGraph : public std::enable_shared_from_this<ImageGraph>
    {
    public:
        ImageGraph(
            const std::filesystem::path&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&,
            const ImageGraphOptions& = ImageGraphOptions());

        ~ImageGraph();

        //! Get the timeline image size.
        const IMATH_NAMESPACE::V2i& getImageSize() const;

        //! Get an image graph for the given time.
        std::shared_ptr<IImageNode> exec(
            const std::shared_ptr<ImageEffectHost>&,
            const OTIO_NS::RationalTime&) const;

    private:
        std::shared_ptr<IImageNode> _track(
            const std::shared_ptr<ImageEffectHost>&,
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>&) const;

        std::shared_ptr<IImageNode> _item(
            const std::shared_ptr<ImageEffectHost>&,
            const OTIO_NS::TimeRange& trimmedRangeInParent,
            const OTIO_NS::RationalTime&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&) const;

        std::shared_ptr<IImageNode> _effects(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Effect> >&,
            const std::shared_ptr<IImageNode>&) const;

        std::filesystem::path _getMediaPath(const std::string&) const;

        std::filesystem::path _path;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        OTIO_NS::RationalTime _globalStartTime;
        ImageGraphOptions _options;
        IMATH_NAMESPACE::V2i _imageSize = IMATH_NAMESPACE::V2i(0, 0);
    };
}
