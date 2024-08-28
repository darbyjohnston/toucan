// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageGraph.h"

#include "Comp.h"
#include "ImageEffectHost.h"
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
    namespace
    {
        const std::string logPrefix = "toucan::ImageGraph";
    }

    ImageGraph::ImageGraph(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const ImageGraphOptions& options) :
        _path(path),
        _timeline(timeline),
        _options(options)
    {
        const auto globalStartTime = timeline->global_start_time();
        _globalStartTime = globalStartTime.has_value() ? globalStartTime.value() :
            OTIO_NS::RationalTime(0.0, timeline->duration().rate());

        // Get the image size from the first clip.
        for (auto clip : _timeline->find_clips())
        {
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::filesystem::path path = _getMediaPath(externalRef->target_url());
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
                const std::filesystem::path path = getSequenceFrame(
                    _getMediaPath(sequenceRef->target_url_base()),
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
        const std::shared_ptr<ImageEffectHost>& host,
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
                if (track->kind() == OTIO_NS::Track::Kind::video)
                {
                    // Process this track.
                    auto trackNode = _track(host, time - _globalStartTime, track);

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
        const std::shared_ptr<ImageEffectHost>& host,
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
                                "Toucan:Dissolve",
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
                                "Toucan:Dissolve",
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
        }

        return out;
    }

    std::shared_ptr<IImageNode> ImageGraph::_item(
        const std::shared_ptr<ImageEffectHost>& host,
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
                const std::filesystem::path path = _getMediaPath(externalRef->target_url());
                auto read = std::make_shared<ReadNode>(path);
                out = read;
            }
            else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(clip->media_reference()))
            {
                const std::filesystem::path path = _getMediaPath(sequenceRef->target_url_base());
                auto read = std::make_shared<SequenceReadNode>(
                    path,
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
            const OTIO_NS::RationalTime timeOffset =
                trimmedRangeInParent.start_time() -
                item->trimmed_range().start_time();
            out->setTimeOffset(timeOffset);
        }

        return out;
    }

    std::shared_ptr<IImageNode> ImageGraph::_effects(
        const std::shared_ptr<ImageEffectHost>& host,
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
            else
            {
                if (auto imageEffect = host->createNode(
                    effect->effect_name(),
                    effect->metadata(),
                    { out }))
                {
                    out = imageEffect;
                }
            }
        }
        return out;
    }

    std::filesystem::path ImageGraph::_getMediaPath(const std::string& url) const
    {
        std::filesystem::path path = splitURLProtocol(url).second;
        if (!path.is_absolute())
        {
            path = _path / path;
        }
        return path;
    }

}
