// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucan/ImageEffectHost.h>
#include <toucan/ImageNode.h>
#include <toucan/MessageLog.h>

#include <opentimelineio/version.h>

namespace toucan
{
    class Effect;
    class IItem;
    class IMediaReference;
    class Timeline;
    class Track;

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
            const std::shared_ptr<Timeline>&,
            const ImageGraphOptions & = ImageGraphOptions());

        ~ImageGraph();

        //! Get the timeline image size.
        const IMATH_NAMESPACE::V2i& getImageSize() const;

        //! Get an image graph for the given time.
        std::shared_ptr<IImageNode> exec(
            const std::shared_ptr<ImageEffectHost>&,
            const OTIO_NS::RationalTime&);

    private:
        std::shared_ptr<IImageNode> _track(
            const std::shared_ptr<ImageEffectHost>&,
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<Track>&);

        std::shared_ptr<IImageNode> _item(
            const std::shared_ptr<ImageEffectHost>&,
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<IItem>&);

        std::shared_ptr<IImageNode> _effects(
            const std::shared_ptr<ImageEffectHost>&,
            const std::vector<std::shared_ptr<Effect> >&,
            const std::shared_ptr<IImageNode>&);

        std::shared_ptr<Timeline> _timeline;
        ImageGraphOptions _options;
        IMATH_NAMESPACE::V2i _imageSize = IMATH_NAMESPACE::V2i(0, 0);
        std::map<std::shared_ptr<IMediaReference>, std::shared_ptr<IImageNode> > _loadCache;
    };
}
