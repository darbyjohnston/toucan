// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageGraph.h"

#include "Comp.h"
#include "ImageHost.h"
#include "Read.h"
#include "TimeWarp.h"
#include "Util.h"

#include <opentimelineio/externalReference.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/generatorReference.h>
#include <opentimelineio/imageSequenceReference.h>
#include <opentimelineio/linearTimeWarp.h>

namespace toucan
{
    ImageGraph::ImageGraph(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const ImageGraphOptions& options) :
        _path(path),
        _timeline(timeline),
        _options(options)
    {
        // Get the image size from the first clip.
        for (auto clip : _timeline->find_clips())
        {
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                std::filesystem::path path = externalRef->target_url();
                if (!path.is_absolute())
                {
                    path = _path / path;
                }
                const OIIO::ImageBuf buf(path.string());
                const auto& spec = buf.spec();
                if (spec.width > 0)
                {
                    _imageSize.x = spec.width;
                    _imageSize.y = spec.height;
                    break;
                }
            }
            else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(clip->media_reference()))
            {
                std::filesystem::path path = sequenceRef->target_url_base();
                if (!path.is_absolute())
                {
                    path = _path / path;
                }
                path = getSequenceFrame(
                    path,
                    sequenceRef->name_prefix(),
                    sequenceRef->start_frame(),
                    sequenceRef->frame_zero_padding(),
                    sequenceRef->name_suffix());
                const OIIO::ImageBuf buf(path.string());
                const auto& spec = buf.spec();
                if (spec.width > 0)
                {
                    _imageSize.x = spec.width;
                    _imageSize.y = spec.height;
                    break;
                }
            }
            else if (auto generatorRef = dynamic_cast<OTIO_NS::GeneratorReference*>(clip->media_reference()))
            {
                auto parameters = generatorRef->parameters();
                auto i = parameters.find("size");
                if (i != parameters.end() && i->second.has_value())
                {
                    anyToVec(std::any_cast<OTIO_NS::AnyVector>(i->second), _imageSize);
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
        const std::shared_ptr<ImageHost>& host,
        const OTIO_NS::RationalTime& time) const
    {
        // Set the background color.
        OTIO_NS::AnyDictionary metaData;
        metaData["size"] = vecToAny(_imageSize);
        metaData["color"] = vecToAny(IMATH_NAMESPACE::V4f(0.F, 0.F, 0.F, 1.F));
        std::shared_ptr<IImageNode> node = host->createNode("Toucan:Fill", metaData);

        // Loop over the tracks.
        auto stack = _timeline->tracks();
        for (const auto& i : stack->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(i))
            {
                // Process this track.
                auto trackNode = _track(host, time, track);

                // Get the track effects.
                const auto& effects = track->effects();
                if (!effects.empty())
                {
                    trackNode = _effects(host, effects, trackNode);
                }

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
        const auto& effects = stack->effects();
        if (!effects.empty())
        {
            node = _effects(host, effects, node);
        }

        return node;
    }

    std::shared_ptr<IImageNode> ImageGraph::_track(
        const std::shared_ptr<ImageHost>& host,
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track) const
    {
        std::shared_ptr<IImageNode> out;

        // Find the items for the given time.
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> item;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> prev;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> prev2;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> next;
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Composable> next2;
        const auto& children = track->children();
        for (size_t i = 0; i < children.size(); ++i)
        {
            if (item = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(children[i]))
            {
                const auto trimmedRangeInParent = item->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    out = _item(
                        host,
                        trimmedRangeInParent.value(),
                        track->transformed_time(time, item),
                        item);
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
        if (item)
        {
            if (auto prevTransition = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Transition>(prev))
            {
                const auto trimmedRangeInParent = prevTransition->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    if (auto prevItem = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(prev2))
                    {
                        auto a = _item(
                            host,
                            prevItem->trimmed_range_in_parent().value(),
                            track->transformed_time(time, prevItem),
                            prevItem);
                        out = _transition(
                            host,
                            prevTransition,
                            trimmedRangeInParent.value(),
                            { a, out });
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
                        auto b = _item(
                            host,
                            nextItem->trimmed_range_in_parent().value(),
                            track->transformed_time(time, nextItem),
                            nextItem);
                        out = _transition(
                            host,
                            nextTransition,
                            trimmedRangeInParent.value(),
                            { out, b });
                    }
                }
            }
        }

        return out;
    }

    std::shared_ptr<IImageNode> ImageGraph::_item(
        const std::shared_ptr<ImageHost>& host,
        const OTIO_NS::TimeRange& trimmedRangeInParent,
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item) const
    {
        std::shared_ptr<IImageNode> out;

        if (auto clip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(item))
        {
            // Get the media reference.
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::string url = externalRef->target_url();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<ReadNode>(path);
                out = read;
            }
            else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(clip->media_reference()))
            {
                const std::string url = sequenceRef->target_url_base();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<SequenceReadNode>(
                    path.string(),
                    sequenceRef->name_prefix(),
                    sequenceRef->name_suffix(),
                    sequenceRef->start_frame(),
                    sequenceRef->frame_step(),
                    sequenceRef->rate(),
                    sequenceRef->frame_zero_padding());
                out = read;
            }
            else if (auto generatorRef = dynamic_cast<OTIO_NS::GeneratorReference*>(clip->media_reference()))
            {
                out = host->createNode(generatorRef->generator_kind(), generatorRef->parameters());
            }
        }
        else if (auto gap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Gap>(item))
        {
            OTIO_NS::AnyDictionary metaData;
            metaData["size"] = vecToAny(_imageSize);
            out = host->createNode("Toucan:Fill", metaData);
        }

        // Get the effects.
        const auto& effects = item->effects();
        if (!effects.empty())
        {
            out = _effects(host, effects, out);
        }
        if (out)
        {
            OTIO_NS::RationalTime timeOffset = trimmedRangeInParent.start_time();
            const auto& sourceRange = item->source_range();
            if (sourceRange.has_value())
            {
                timeOffset -= sourceRange.value().start_time();
            }
            out->setTimeOffset(timeOffset);
        }

        return out;
    }

    std::shared_ptr<IImageNode> ImageGraph::_transition(
        const std::shared_ptr<ImageHost>& host,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Transition>& otioTransition,
        const OTIO_NS::TimeRange& trimmedRangeInParent,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) const
    {
        OTIO_NS::AnyDictionary metaData;
        metaData["range"] = trimmedRangeInParent;
        return host->createNode("Toucan:" + otioTransition->transition_type(), metaData);
    }

    std::shared_ptr<IImageNode> ImageGraph::_effects(
        const std::shared_ptr<ImageHost>& host,
        const std::vector<OTIO_NS::SerializableObject::Retainer<OTIO_NS::Effect> >& effects,
        const std::shared_ptr<IImageNode>& input) const
    {
        std::shared_ptr<IImageNode> out = input;
        for (const auto& effect : effects)
        {
            if (auto iEffect = dynamic_cast<IEffect*>(effect.value))
            {
                auto effectNode = iEffect->createNode(host, { out });
                out = effectNode;
            }
            else if (auto linearTimeWarp = dynamic_cast<OTIO_NS::LinearTimeWarp*>(effect.value))
            {
                auto linearTimeWarpNode = std::make_shared<LinearTimeWarpNode>(
                    static_cast<float>(linearTimeWarp->time_scalar()),
                    std::vector<std::shared_ptr<IImageNode> >{ out });
                out = linearTimeWarpNode;
            }
        }
        return out;
    }
}
