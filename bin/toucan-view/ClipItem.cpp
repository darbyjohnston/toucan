// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ClipItem.h"

#include <dtk/ui/DrawUtil.h>
#include <dtk/core/RenderUtil.h>

namespace toucan
{
    void ClipItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip,
        const dtk::Color4F& color,
        const std::shared_ptr<IWidget>& parent)
    {
        auto opt = clip->trimmed_range_in_parent();
        IItem::_init(
            context,
            app,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(clip),
            opt.has_value() ? opt.value() : OTIO_NS::TimeRange(),
            "toucan::ClipItem",
            parent);

        _setMousePressEnabled(0, 0);

        _clip = clip;
        _text = !clip->name().empty() ? clip->name() : "Clip";
        _color = color;

        setTooltip(_text);
    }
    
    ClipItem::~ClipItem()
    {}

    std::shared_ptr<ClipItem> ClipItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Clip>& clip,
        const dtk::Color4F& color,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<ClipItem>();
        out->_init(context, app, clip, color, parent);
        return out;
    }

    void ClipItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
            _size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
            _size.borderFocus = event.style->getSizeRole(dtk::SizeRole::BorderFocus, event.displayScale);
            _size.fontInfo = event.style->getFontRole(dtk::FontRole::Label , event.displayScale);
            _size.fontMetrics = event.fontSystem->getMetrics(_size.fontInfo);
            _size.textSize = event.fontSystem->getSize(_text, _size.fontInfo);
            _draw.glyphs.clear();
        }
        dtk::Size2I sizeHint(
            _timeRange.duration().rescaled_to(1.0).value() * _scale,
            _size.textSize.h + _size.margin * 2 + _size.borderFocus * 2);
        _setSizeHint(sizeHint);
    }

    void ClipItem::clipEvent(const dtk::Box2I& clipRect, bool clipped)
    {
        IItem::clipEvent(clipRect, clipped);
        if (clipped)
        {
            _draw.glyphs.clear();
        }
    }
    
    void ClipItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();

        if (_selected)
        {
            event.render->drawMesh(
                dtk::border(g, _size.borderFocus),
                event.style->getColorRole(dtk::ColorRole::Yellow));
        }

        const dtk::Box2I g2 = dtk::margin(g, -_size.borderFocus);
        event.render->drawRect(g2, _color);

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
            dtk::V2F(g3.min.x, g3.min.y + g3.h() / 2 - _size.fontMetrics.lineHeight / 2),
            event.style->getColorRole(dtk::ColorRole::Text));
    }
}
