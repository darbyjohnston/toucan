// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TrackItem.h"

#include "ClipItem.h"
#include "GapItem.h"

namespace toucan
{
    void TrackItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        auto opt = track->trimmed_range_in_parent();
        IItem::_init(
            context,
            app,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(track),
            opt.has_value() ? opt.value() : OTIO_NS::TimeRange(),
            "toucan::TrackItem",
            parent);

        _track = track;

        for (const auto& child : track->children())
        {
            if (auto clip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(child))
            {
                ClipItem::create(context, app, clip, shared_from_this());
            }
            else if (auto gap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Gap>(child))
            {
                GapItem::create( context, app, gap, shared_from_this());
            }
        }
    }

    TrackItem::~TrackItem()
    {}

    std::shared_ptr<TrackItem> TrackItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<TrackItem>();
        out->_init(context, app, track, parent);
        return out;
    }

    void TrackItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        const dtk::Box2I g = dtk::margin(value, 0, -_size.margin, 0, -_size.margin);
        dtk::V2I pos = g.min;
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& sizeHint = child->getSizeHint();
            child->setGeometry(dtk::Box2I(pos.x, pos.y, sizeHint.w, g.h()));
            pos.x += sizeHint.w;
        }
    }

    void TrackItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
        }
        dtk::Size2I sizeHint(
            _timeRange.duration().rescaled_to(1.0).value() * _scale,
            0);
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& childSizeHint = child->getSizeHint();
            sizeHint.h = std::max(sizeHint.h, childSizeHint.h);
        }
        sizeHint.h += _size.margin * 2;
        _setSizeHint(sizeHint);
    }
}
