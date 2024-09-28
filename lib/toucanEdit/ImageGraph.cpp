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
                        timeline->getPath().parent_path(),
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
        auto stack = _timeline->getStack();
        for (const auto& track : stack->findAll<Track>(Find::Shallow))
        {
            if (TrackKind::video == track->getKind())
            {
                // Process this track.
                auto trackNode = _track(host, stack->transform(time, track), track);

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
        std::shared_ptr<IImageNode> out;

        // Find the items for the given time.
        std::shared_ptr<IItem> item;
        std::shared_ptr<IItem> prev;
        std::shared_ptr<IItem> prev2;
        std::shared_ptr<IItem> next;
        std::shared_ptr<IItem> next2;
        const auto& children = track->getChildren();
        for (size_t i = 0; i < children.size(); ++i)
        {
            if ((item = std::dynamic_pointer_cast<IItem>(children[i])))
            {
                const OTIO_NS::RationalTime itemTime = track->transform(time, item);
                const OTIO_NS::TimeRange& range = item->getRange();
                if (range.contains(itemTime))
                {
                    out = _item(host, itemTime, item);
                    if (i > 0)
                    {
                        prev = children[i - 1];
                    }
                    if (i > 1)
                    {
                        prev2 = children[i - 2];
                    }
                    if (i < (children.size() - 1))
                    {
                        next = children[i + 1];
                    }
                    if (children.size() > 1 && i < (children.size() - 2))
                    {
                        next2 = children[i + 2];
                    }
                    break;
                }
            }
        }

        // Handle transitions.
        /*if (item)
        {
            if (auto prevTransition = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Transition>(prev))
            {
                const auto trimmedRangeInParent = prevTransition->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    if (auto prevItem = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(prev2))
                    {
                        const double value =
                            (time - trimmedRangeInParent.value().start_time()).value() /
                            trimmedRangeInParent.value().duration().value();

                        auto metaData = prevTransition->metadata();
                        metaData["value"] = value;
                        auto node = host->createNode(
                            prevTransition->transition_type(),
                            metaData);
                        if (!node)
                        {
                            node = host->createNode(
                                "toucan:Dissolve",
                                metaData);
                        }
                        if (node)
                        {
                            auto a = _item(
                                host,
                                prevItem->trimmed_range_in_parent().value(),
                                track->transformed_time(time, prevItem),
                                prevItem);
                            node->setInputs({ a, out });
                            out = node;
                        }
                    }
                }
            }
            if (auto nextTransition = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Transition>(next))
            {
                const auto trimmedRangeInParent = nextTransition->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    if (auto nextItem = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(next2))
                    {
                        const double value =
                            (time - trimmedRangeInParent.value().start_time()).value() /
                            trimmedRangeInParent.value().duration().value();

                        auto metaData = nextTransition->metadata();
                        metaData["value"] = value;
                        auto node = host->createNode(
                            nextTransition->transition_type(),
                            metaData);
                        if (!node)
                        {
                            node = host->createNode(
                                "toucan:Dissolve",
                                metaData);
                        }
                        if (node)
                        {
                            auto b = _item(
                                host,
                                nextItem->trimmed_range_in_parent().value(),
                                track->transformed_time(time, nextItem),
                                nextItem);
                            node->setInputs({ out, b });
                            out = node;
                        }
                    }
                }
            }
        }*/

        return out;
    }

    std::shared_ptr<IImageNode> ImageGraph::_item(
        const std::shared_ptr<ImageEffectHost>& host,
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<IItem>& item)
    {
        std::shared_ptr<IImageNode> out;

        if (auto clip = std::dynamic_pointer_cast<Clip>(item))
        {
            // Get the media reference.
            auto ref = clip->getMediaReference();
            if (auto externalRef = std::dynamic_pointer_cast<ExternalReference>(ref))
            {
                auto i = _loadCache.find(externalRef);
                if (i != _loadCache.end())
                {
                    out = i->second;
                }
                else
                {
                    const std::filesystem::path path = getMediaPath(
                        _timeline->getPath().parent_path(),
                        externalRef->getURL());
                    auto read = std::make_shared<ReadNode>(path);
                    out = read;
                    _loadCache[externalRef] = read;
                }
            }
        }
        else if (auto gap = std::dynamic_pointer_cast<Gap>(item))
        {
            OTIO_NS::AnyDictionary metaData;
            metaData["size"] = vecToAny(_imageSize);
            out = host->createNode("toucan:Fill", metaData);
        }

        // Get the effects.
        /*const auto& effects = item->effects();
        if (!effects.empty())
        {
            out = _effects(host, effects, out);
        }
        if (out)
        {
            const OTIO_NS::RationalTime timeOffset =
                trimmedRangeInParent.start_time() -
                item->trimmed_range().start_time();
            out->setTimeOffset(timeOffset);
        }*/

        return out;
    }
}
