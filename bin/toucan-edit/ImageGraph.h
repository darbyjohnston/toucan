// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageNode.h>
#include <toucan/MessageLog.h>

#include <opentimelineio/version.h>

namespace toucan
{
    class Timeline;

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
        std::shared_ptr<Timeline> _timeline;
        ImageGraphOptions _options;
        IMATH_NAMESPACE::V2i _imageSize = IMATH_NAMESPACE::V2i(0, 0);
    };
}
