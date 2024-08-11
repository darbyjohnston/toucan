// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimelineTraverse.h"

#include "CompOp.h"
#include "FillOp.h"
#include "LinearTimeWarpOp.h"
#include "ReadOp.h"
#include "SequenceReadOp.h"
#include "TransitionOp.h"

#include <opentimelineio/externalReference.h>
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
        // The first clip sets the timeline resolution.
        for (auto clip : _timeline->find_clips())
        {
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::string url = externalRef->target_url();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<ReadOp>(path);
                const auto buf = read->exec(OTIO_NS::RationalTime(0.0, 1.0));
                const auto& spec = buf.spec();
                if (spec.width > 0)
                {
                    _size.x = spec.width;
                    _size.y = spec.height;
                    break;
                }
            }
            else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(clip->media_reference()))
            {
                const std::string url = sequenceRef->target_url_base();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<SequenceReadOp>(
                    path.string(),
                    sequenceRef->name_prefix(),
                    sequenceRef->name_suffix(),
                    sequenceRef->start_frame(),
                    sequenceRef->frame_step(),
                    sequenceRef->rate(),
                    sequenceRef->frame_zero_padding());
                const auto buf = read->exec(OTIO_NS::RationalTime(0.0, 1.0));
                const auto& spec = buf.spec();
                if (spec.width > 0)
                {
                    _size.x = spec.width;
                    _size.y = spec.height;
                    break;
                }
            }
        }
    }

    TimelineTraverse::~TimelineTraverse()
    {}

    std::shared_ptr<IImageOp> TimelineTraverse::exec(const OTIO_NS::RationalTime& time)
    {
        _op = std::make_shared<FillOp>(FillData{ _size });
        for (const auto& i : _timeline->tracks()->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(i))
            {
                _track(time, track);
            }
        }
        return _op;
    }

    namespace
    {
        OTIO_NS::Composable* prevComposable(OTIO_NS::Composable* composable)
        {
            OTIO_NS::Composable* out = nullptr;
            if (auto parent = composable->parent())
            {
                const auto children = parent->children();
                const auto i = std::find(children.begin(), children.end(), composable);
                if (i != children.end())
                {
                    const size_t offset = i - children.begin();
                    if (offset > 0)
                    {
                        out = children[offset - 1].value;
                    }
                }
            }
            return out;
        }

        OTIO_NS::Composable* nextComposable(OTIO_NS::Composable* composable)
        {
            OTIO_NS::Composable* out = nullptr;
            if (auto parent = composable->parent())
            {
                const auto children = parent->children();
                const auto i = std::find(children.begin(), children.end(), composable);
                if (i != children.end())
                {
                    const size_t offset = i - children.begin();
                    if (offset < children.size() - 1)
                    {
                        out = children[offset + 1].value;
                    }
                }
            }
            return out;
        }
    }

    void TimelineTraverse::_track(
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track)
    {
        for (const auto& i : track->children())
        {
            std::shared_ptr<IImageOp> op;
            if (auto item = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(i))
            {
                const auto trimmedRangeInParent = item->trimmed_range_in_parent();
                if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                {
                    op = _item(
                        trimmedRangeInParent.value(),
                        track->transformed_time(time, item),
                        item);
                }
            }

            if (op)
            {
                if (auto prevTransition = dynamic_cast<OTIO_NS::Transition*>(prevComposable(i.value)))
                {
                    const auto trimmedRangeInParent = prevTransition->trimmed_range_in_parent();
                    if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                    {
                        if (auto prevItem = dynamic_cast<OTIO_NS::Item*>(prevComposable(prevTransition)))
                        {
                            auto a = _item(
                                prevItem->trimmed_range_in_parent().value(),
                                track->transformed_time(time, prevItem),
                                prevItem);
                            op = std::make_shared<TransitionOp>(
                                trimmedRangeInParent.value(),
                                std::vector<std::shared_ptr<IImageOp> >{ a, op });
                        }
                    }
                }
                else if (auto nextTransition = dynamic_cast<OTIO_NS::Transition*>(nextComposable(i.value)))
                {
                    const auto trimmedRangeInParent = nextTransition->trimmed_range_in_parent();
                    if (trimmedRangeInParent.has_value() && trimmedRangeInParent.value().contains(time))
                    {
                        if (auto nextItem = dynamic_cast<OTIO_NS::Item*>(nextComposable(nextTransition)))
                        {
                            auto b = _item(
                                nextItem->trimmed_range_in_parent().value(),
                                track->transformed_time(time, nextItem),
                                nextItem);
                            op = std::make_shared<TransitionOp>(
                                trimmedRangeInParent.value(),
                                std::vector<std::shared_ptr<IImageOp> >{ op, b });
                        }
                    }
                }
            }

            // Composite.
            std::vector<std::shared_ptr<IImageOp> > ops;
            if (op)
            {
                ops.push_back(op);
            }
            if (_op)
            {
                ops.push_back(_op);
            }
            auto comp = std::make_shared<CompOp>(ops);
            comp->setPremult(true);
            _op = comp;
        }
    }

    std::shared_ptr<IImageOp> TimelineTraverse::_item(
        const OTIO_NS::TimeRange& trimmedRangeInParent,
        const OTIO_NS::RationalTime& time,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item)
    {
        std::shared_ptr<IImageOp> out;

        const OTIO_NS::TimeRange trimmedRange = item->trimmed_range();
        const OTIO_NS::RationalTime timeOffset = trimmedRangeInParent.start_time();

        if (auto clip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(item))
        {
            // Get the media reference.
            if (auto externalRef = dynamic_cast<OTIO_NS::ExternalReference*>(clip->media_reference()))
            {
                const std::string url = externalRef->target_url();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<ReadOp>(path);
                out = read;
            }
            else if (auto sequenceRef = dynamic_cast<OTIO_NS::ImageSequenceReference*>(clip->media_reference()))
            {
                const std::string url = sequenceRef->target_url_base();
                const std::filesystem::path path = _path / url;
                auto read = std::make_shared<SequenceReadOp>(
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
        else if (auto gap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(item))
        {
            out = std::make_shared<FillOp>(FillData{ _size });
        }

        // Create the effects.
        for (const auto& effect : item->effects())
        {
            if (auto iEffect = dynamic_cast<IEffect*>(effect.value))
            {
                auto effectOp = iEffect->createOp({ out });
                out = effectOp;
            }
            else if (auto linearTimeWarp = dynamic_cast<OTIO_NS::LinearTimeWarp*>(effect.value))
            {
                auto linearTimeWarpOp = std::make_shared<LinearTimeWarpOp>(
                    static_cast<float>(linearTimeWarp->time_scalar()),
                    std::vector<std::shared_ptr<IImageOp> >{ out });
                out = linearTimeWarpOp;
            }
        }
        if (out)
        {
            out->setTimeOffset(timeOffset);
        }

        return out;
    }
}
