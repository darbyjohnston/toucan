// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ImageGraph.h"

#include "Clip.h"
#include "Effects.h"
#include "Gap.h"
#include "MediaReferences.h"
#include "Stack.h"
#include "Timeline.h"
#include "Track.h"
#include "Transitions.h"

#include <toucan/Comp.h>
#include <toucan/Read.h>
#include <toucan/TimeWarp.h>
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
            if (auto ref = clip->getMediaReference())
            {
                if (auto externalRef = std::dynamic_pointer_cast<ExternalReference>(ref))
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
                else if (auto sequenceRef = std::dynamic_pointer_cast<ImageSequenceReference>(ref))
                {
                    const std::filesystem::path path = getSequenceFrame(
                        getMediaPath(timeline->getPath().parent_path(), sequenceRef->getTargetURLBase()),
                        sequenceRef->getNamePrefix(),
                        sequenceRef->getStartFrame(),
                        sequenceRef->getFrameZeroPadding(),
                        sequenceRef->getNameSuffix());
                    const OIIO::ImageBuf buf(path.string());
                    const auto& spec = buf.spec();
                    if (spec.width > 0)
                    {
                        _imageSize.x = spec.width;
                        _imageSize.y = spec.height;
                        break;
                    }
                }
                else if (auto generatorRef = std::dynamic_pointer_cast<GeneratorReference>(ref))
                {
                    auto parameters = generatorRef->getParameters();
                    auto i = parameters.find("size");
                    if (i != parameters.end() && i->second.has_value())
                    {
                        anyToVec(std::any_cast<OTIO_NS::AnyVector>(i->second), _imageSize);
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
                const OTIO_NS::RationalTime trackTime = stack->transform(time, track);
                auto trackNode = _track(host, trackTime, track);

                // Get the track effects.
                const auto& effects = track->getEffects();
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
        const auto& effects = stack->getEffects();
        if (!effects.empty())
        {
            node = _effects(host, effects, node);
        }

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
            item = children[i];
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

        // Handle transitions.
        if (item)
        {
            if (auto prevTransition = std::dynamic_pointer_cast<Transition>(prev))
            {
                OTIO_NS::TimeRange rangeInParent = prevTransition->transform(
                    OTIO_NS::TimeRange(
                        -prevTransition->getInOffset(),
                        prevTransition->getInOffset() + prevTransition->getOutOffset()),
                    track);
                if (rangeInParent.contains(time) && prev2)
                {
                    const double value =
                        (time - rangeInParent.start_time()).value() /
                        rangeInParent.duration().value();

                    auto metaData = prevTransition->getMetadata();
                    metaData["value"] = value;
                    auto node = host->createNode(
                        prevTransition->getTransitionType(),
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
                            track->transform(time, prev2),
                            prev2);
                        node->setInputs({ a, out });
                        out = node;
                    }
                }
            }
            if (auto nextTransition = std::dynamic_pointer_cast<Transition>(next))
            {
                OTIO_NS::TimeRange rangeInParent = nextTransition->transform(
                    OTIO_NS::TimeRange(
                        -nextTransition->getInOffset(),
                        nextTransition->getInOffset() + nextTransition->getOutOffset()),
                    track);
                if (rangeInParent.contains(time) && next2)
                {
                    const double value =
                        (time - rangeInParent.start_time()).value() /
                        rangeInParent.duration().value();

                    auto metaData = nextTransition->getMetadata();
                    metaData["value"] = value;
                    auto node = host->createNode(
                        nextTransition->getTransitionType(),
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
                            track->transform(time, next2),
                            next2);
                        node->setInputs({ out, b });
                        out = node;
                    }
                }
            }
        }

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
            else if (auto sequenceRef = std::dynamic_pointer_cast<ImageSequenceReference>(ref))
            {
                const std::filesystem::path path = getMediaPath(
                    _timeline->getPath().parent_path(),
                    sequenceRef->getTargetURLBase());
                auto read = std::make_shared<SequenceReadNode>(
                    path,
                    sequenceRef->getNamePrefix(),
                    sequenceRef->getNameSuffix(),
                    sequenceRef->getStartFrame(),
                    sequenceRef->getFrameStep(),
                    sequenceRef->getRate(),
                    sequenceRef->getFrameZeroPadding());
                out = read;
            }
            else if (auto generatorRef = std::dynamic_pointer_cast<GeneratorReference>(ref))
            {
                out = host->createNode(
                    generatorRef->getGeneratorKind(),
                    generatorRef->getParameters());
            }
        }
        else if (auto gap = std::dynamic_pointer_cast<Gap>(item))
        {
            OTIO_NS::AnyDictionary metaData;
            metaData["size"] = vecToAny(_imageSize);
            out = host->createNode("toucan:Fill", metaData);
        }

        // Get the effects.
        const auto& effects = item->getEffects();
        if (!effects.empty())
        {
            out = _effects(host, effects, out);
        }
        if (out)
        {
            const OTIO_NS::RationalTime timeOffset = item->transform(
                item->getRange().start_time(),
                _timeline->getStack());
            out->setTimeOffset(timeOffset);
        }

        return out;
    }

    std::shared_ptr<IImageNode> ImageGraph::_effects(
        const std::shared_ptr<ImageEffectHost>& host,
        const std::vector<std::shared_ptr<Effect> >& effects,
        const std::shared_ptr<IImageNode>& input)
    {
        std::shared_ptr<IImageNode> out = input;
        for (const auto& effect : effects)
        {
            if (auto linearTimeWarp = std::dynamic_pointer_cast<LinearTimeWarp>(effect))
            {
                auto linearTimeWarpNode = std::make_shared<LinearTimeWarpNode>(
                    static_cast<float>(linearTimeWarp->getTimeScalar()),
                    std::vector<std::shared_ptr<IImageNode> >{ out });
                out = linearTimeWarpNode;
            }
            else
            {
                if (auto imageEffect = host->createNode(
                    effect->getEffectName(),
                    effect->getMetadata(),
                    { out }))
                {
                    out = imageEffect;
                }
            }
        }
        return out;
    }
}
