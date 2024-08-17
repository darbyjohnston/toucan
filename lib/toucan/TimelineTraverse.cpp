// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineTraverse.h"

#include "Comp.h"
#include "Fill.h"
#include "LinearTimeWarp.h"
#include "Read.h"
#include "SequenceRead.h"
#include "Transition.h"

#include <opentimelineio/externalReference.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/imageSequenceReference.h>
#include <opentimelineio/linearTimeWarp.h>

namespace toucan
{
    TimelineTraverse::TimelineTraverse(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline) :
        _path(path),
        _timeline(timeline)
    {
        // Get the image size from the first clip.
        for (auto clip : _timeline->find_clips())
        {
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::string url = externalRef->target_url();
                const std::filesystem::path path = _path / url;
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
                const std::string url = sequenceRef->target_url_base();
                const std::filesystem::path path = _path / url;
                std::stringstream ss;
                ss << path.string() <<
                    sequenceRef->name_prefix() <<
                    std::setw(sequenceRef->frame_zero_padding()) << std::setfill('0') << sequenceRef->start_frame() <<
                    sequenceRef->name_suffix();
                const OIIO::ImageBuf buf(ss.str());
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

    TimelineTraverse::~TimelineTraverse()
    {}

    const IMATH_NAMESPACE::V2d& TimelineTraverse::getImageSize() const
    {
        return _imageSize;
    }

    std::shared_ptr<IImageNode> TimelineTraverse::exec(const OTIO_NS::RationalTime& time) const
    {
        std::shared_ptr<IImageNode> node = std::make_shared<FillNode>(
            FillData{ _imageSize, IMATH_NAMESPACE::V4f(0.F, 0.F, 0.F, 1.F )});
        for (const auto& i : _timeline->tracks()->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(i))
            {
                auto trackNode = _track(time, track);

                // Composite over the previous track.
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
        return node;
    }

    std::shared_ptr<IImageNode> TimelineTraverse::_track(
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
                            prevItem->trimmed_range_in_parent().value(),
                            track->transformed_time(time, prevItem),
                            prevItem);
                        out = std::make_shared<TransitionNode>(
                            trimmedRangeInParent.value(),
                            std::vector<std::shared_ptr<IImageNode> >{ a, out });
                    }
                }
            }
            else if (auto nextTransition = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Transition>(next))
            {
                const auto trimmedRangeInParent = nextTransition->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    if (auto nextItem = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(next2))
                    {
                        auto b = _item(
                            nextItem->trimmed_range_in_parent().value(),
                            track->transformed_time(time, nextItem),
                            nextItem);
                        out = std::make_shared<TransitionNode>(
                            trimmedRangeInParent.value(),
                            std::vector<std::shared_ptr<IImageNode> >{ out, b });
                    }
                }
            }
        }

        return out;
    }

    std::shared_ptr<IImageNode> TimelineTraverse::_item(
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
        }
        else if (auto gap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Gap>(item))
        {
            out = std::make_shared<FillNode>(FillData{ _imageSize });
        }

        // Get the effects.
        for (const auto& effect : item->effects())
        {
            if (auto iEffect = dynamic_cast<IEffect*>(effect.value))
            {
                auto effectNode = iEffect->createNode({ out });
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
}
