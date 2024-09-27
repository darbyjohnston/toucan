// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageGraph.h"

#include "Clip.h"
#include "Gap.h"
#include "MediaReference.h"
#include "Stack.h"
#include "Timeline.h"
#include "Track.h"

#include <toucan/Comp.h>
#include <toucan/Read.h>
#include <toucan/Util.h>

namespace toucan
{
    ImageGraph::ImageGraph(
        const std::shared_ptr<Timeline>& timeline,
        const ImageGraphOptions& options) :
        _timeline(timeline),
        _options(options)
    {
        // Get the image size from the first clip.
        for (const auto& clip : timeline->getStack()->findAll<Clip>())
        {
            auto refs = clip->getMediaReferences();
            auto i = refs.find(clip->getActiveMediaReference());
            if (i == refs.end())
            {
                i = refs.begin();
            }
            if (i != refs.end())
            {
                if (auto externalRef = std::dynamic_pointer_cast<ExternalReference>(i->second))
                {
                    const std::filesystem::path path = getMediaPath(
                        timeline->getPath(),
                        externalRef->getURL());
                    const OIIO::ImageBuf buf(path.string());
                    const auto& spec = buf.spec();
                    if (spec.width > 0)
                    {
                        _imageSize.x = spec.width;
                        _imageSize.y = spec.height;
                        break;
                    }
                }
            }
        }
    }

    ImageGraph::~ImageGraph()
    {}

    const IMATH_NAMESPACE::V2i& ImageGraph::getImageSize() const
    {
        return _imageSize;
    }

    std::shared_ptr<IImageNode> ImageGraph::exec(
        const std::shared_ptr<ImageEffectHost>& host,
        const OTIO_NS::RationalTime& time)
    {
        // Set the background color.
        OTIO_NS::AnyDictionary metaData;
        metaData["size"] = vecToAny(_imageSize);
        metaData["color"] = vecToAny(IMATH_NAMESPACE::V4f(0.F, 0.F, 0.F, 1.F));
        std::shared_ptr<IImageNode> node = host->createNode("toucan:Fill", metaData);

        // Loop over the tracks.
        for (const auto& track : _timeline->getStack()->findAll<Track>(Find::Shallow))
        {
            if (TrackKind::video == track->getKind())
            {
                // Process this track.
                auto trackNode = _track(host, time, track);

                // Get the track effects.
                //const auto& effects = track->effects();
                //if (!effects.empty())
                //{
                //    trackNode = _effects(host, effects, trackNode);
                //}

                // Composite this track over the previous track.
                std::vector<std::shared_ptr<IImageNode> > nodes;
                if (trackNode)
                {
                    nodes.push_back(trackNode);
                }
                if (node)
                {
                    nodes.push_back(node);
                }
                auto comp = std::make_shared<CompNode>(nodes);
                comp->setPremult(true);
                node = comp;
            }
        }


        // Get the stack effects.
        //const auto& effects = stack->effects();
        //if (!effects.empty())
        //{
        //    node = _effects(host, effects, node);
        //}

        return node;
    }

    std::shared_ptr<IImageNode> ImageGraph::_track(
        const std::shared_ptr<ImageEffectHost>& host,
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<Track>& track)
    {
        return nullptr;
    }

    std::shared_ptr<IImageNode> ImageGraph::_item(
        const std::shared_ptr<ImageEffectHost>& host,
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<IItem>& item)
    {
        return nullptr;
    }
}
