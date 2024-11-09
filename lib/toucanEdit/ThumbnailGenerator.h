// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/core/Image.h>

#include <opentimelineio/anyVector.h>

#include <future>

namespace toucan
{
    class ImageEffectHost;
    class ImageGraph;
    class Timeline;

    struct Thumbnail
    {
        OTIO_NS::RationalTime time;
        std::shared_ptr<dtk::Image> image;
    };

    class ThumbnailGenerator : public std::enable_shared_from_this<ThumbnailGenerator>
    {
    public:
        ThumbnailGenerator(
            const std::shared_ptr<Timeline>&,
            const std::shared_ptr<ImageEffectHost>&);

        ~ThumbnailGenerator();

        float getAspect() const;

        std::future<Thumbnail> getThumbnail(
            const OTIO_NS::RationalTime&,
            int height);

    private:
        std::shared_ptr<Timeline> _timeline;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<ImageGraph> _graph;
        float _aspect = 1.F;
    };
}
