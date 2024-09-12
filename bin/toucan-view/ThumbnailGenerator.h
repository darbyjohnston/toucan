// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucan/ImageEffectHost.h>
#include <toucan/ImageGraph.h>

#include <dtk/core/Image.h>

#include <opentimelineio/timeline.h>

#include <filesystem>
#include <future>

namespace toucan
{
    struct Thumbnail
    {
        OTIO_NS::RationalTime time;
        std::shared_ptr<dtk::Image> image;
    };

    class ThumbnailGenerator : public std::enable_shared_from_this<ThumbnailGenerator>
    {
    public:
        ThumbnailGenerator(
            const std::filesystem::path&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>&,
            const std::shared_ptr<ImageEffectHost>&);

        ~ThumbnailGenerator();

        float getAspect() const;

        std::future<Thumbnail> getThumbnail(
            const OTIO_NS::RationalTime&,
            int height);

    private:
        std::filesystem::path _path;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> _timeline;
        std::shared_ptr<ImageEffectHost> _host;
        std::shared_ptr<ImageGraph> _graph;
        float _aspect = 1.F;
    };
}
