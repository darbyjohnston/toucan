// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TrackWidget.h"

#include "Track.h"

#include <dtk/ui/DrawUtil.h>
#include <dtk/core/RenderUtil.h>

namespace toucan
{
    void TrackWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        IItemWidget::_init(
            context,
            app,
            track,
            track->getRange(),
            "toucan::TrackWidget",
            parent);

        _track = track;
        _text = track->getName();
        if (_text.empty())
        {
            _text = track->getKind();
        }
        _color = TrackKind::video == track->getKind() ?
            dtk::Color4F(.2F, .2F, .3F) :
            dtk::Color4F(.2F, .3F, .2F);

        setTooltip(_text);
    }

    TrackWidget::~TrackWidget()
    {}

    std::shared_ptr<TrackWidget> TrackWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<TrackWidget>();
        out->_init(context, app, track, parent);
        return out;
    }

    void TrackWidget::setGeometry(const dtk::Box2I& value)
    {
        IItemWidget::setGeometry(value);
        const dtk::Box2I& g = getGeometry();
        dtk::V2I pos = g.min;
        pos.y += _size.fontMetrics.lineHeight + _size.margin * 2 + _size.border * 2;
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& sizeHint = child->getSizeHint();
            child->setGeometry(dtk::Box2I(pos.x, pos.y, sizeHint.w, sizeHint.h));
            pos.x += sizeHint.w;
        }
    }

    void TrackWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItemWidget::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginInside, event.displayScale);
            _size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
            _size.fontInfo = event.style->getFontRole(dtk::FontRole::Label, event.displayScale);
            _size.fontMetrics = event.fontSystem->getMetrics(_size.fontInfo);
            _size.textSize = event.fontSystem->getSize(_text, _size.fontInfo);
            _draw.glyphs.clear();
        }
        dtk::Size2I sizeHint(
            _timeRange.duration().rescaled_to(1.0).value() * _scale,
            0);
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& childSizeHint = child->getSizeHint();
            sizeHint.h = std::max(sizeHint.h, childSizeHint.h);
        }
        sizeHint.h += _size.textSize.h + _size.margin * 2 + _size.border * 4;
        _setSizeHint(sizeHint);
    }

    void TrackWidget::clipEvent(const dtk::Box2I& clipRect, bool clipped)
    {
        IItemWidget::clipEvent(clipRect, clipped);
        if (clipped)
        {
            _draw.glyphs.clear();
        }
    }

    void TrackWidget::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItemWidget::drawEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();

        const dtk::Box2I g2(
            g.min.x + _size.border,
            g.min.y,
            g.w() - _size.border * 2,
            _size.fontMetrics.lineHeight + _size.margin * 2);
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
            g3.min,
            event.style->getColorRole(dtk::ColorRole::Text));
    }
}
