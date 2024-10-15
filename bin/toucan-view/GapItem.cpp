// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "GapItem.h"

#include <dtk/ui/DrawUtil.h>
#include <dtk/core/Random.h>
#include <dtk/core/RenderUtil.h>

#include <opentimelineio/gap.h>

namespace toucan
{
    void GapItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap>& gap,
        const std::shared_ptr<IWidget>& parent)
    {
        auto opt = gap->trimmed_range_in_parent();
        IItem::_init(
            context,
            app,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(gap),
            opt.has_value() ? opt.value() : OTIO_NS::TimeRange(),
            "toucan::ClipItem",
            parent);

        setTooltip(gap->name());

        _gap = gap;
        _text = !gap->name().empty() ? gap->name() : "Gap";
        _color = dtk::Color4F(.3F, .3F, .3F);

        setTooltip(_text);
    }
    
    GapItem::~GapItem()
    {}

    std::shared_ptr<GapItem> GapItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Gap>& gap,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<GapItem>();
        out->_init(context, app, gap, parent);
        return out;
    }

    void GapItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
            _size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
            _size.fontInfo = event.style->getFontRole(dtk::FontRole::Label , event.displayScale);
            _size.fontMetrics = event.fontSystem->getMetrics(_size.fontInfo);
            _size.textSize = event.fontSystem->getSize(_text, _size.fontInfo);
            _draw.glyphs.clear();
        }
        dtk::Size2I sizeHint(
            _timeRange.duration().rescaled_to(1.0).value() * _scale,
            _size.textSize.h + _size.margin * 2 + _size.border * 2);
        _setSizeHint(sizeHint);
    }

    void GapItem::clipEvent(const dtk::Box2I& clipRect, bool clipped)
    {
        IItem::clipEvent(clipRect, clipped);
        if (clipped)
        {
            _draw.glyphs.clear();
        }
    }

    void GapItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();

        const dtk::Box2I g2 = dtk::margin(g, -_size.border, 0, -_size.border, 0);
        event.render->drawRect(
            g2,
            _selected ? event.style->getColorRole(dtk::ColorRole::Yellow) : _color);

        const dtk::Box2I g3 = dtk::margin(g2, -_size.margin);
        if (!_text.empty() && _draw.glyphs.empty())
        {
            _draw.glyphs = event.fontSystem->getGlyphs(_text, _size.fontInfo);
        }
        dtk::ClipRectEnabledState clipRectEnabledState(event.render);
        dtk::ClipRectState clipRectState(event.render);
        event.render->setClipRectEnabled(true);
        event.render->setClipRect(intersect(g3, drawRect));
        event.render->drawText(
            _draw.glyphs,
            _size.fontMetrics,
            dtk::V2I(g3.min.x, g3.min.y + g3.h() / 2 - _size.fontMetrics.lineHeight / 2),
            event.style->getColorRole(dtk::ColorRole::Text));
    }
}
