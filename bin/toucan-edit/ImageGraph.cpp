// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageGraph.h"

namespace toucan
{
    ImageGraph::ImageGraph(
        const std::shared_ptr<Timeline>& timeline,
        const ImageGraphOptions& options) :
        _timeline(timeline),
        _options(options)
    {}

    ImageGraph::~ImageGraph()
    {}

    const IMATH_NAMESPACE::V2i& ImageGraph::getImageSize() const
    {
        return _imageSize;
    }

    std::shared_ptr<IImageNode> ImageGraph::exec(
        const std::shared_ptr<ImageEffectHost>&,
        const OTIO_NS::RationalTime&)
    {
        return nullptr;
    }
}
