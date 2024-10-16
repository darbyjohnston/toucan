// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "GapWidget.h"

#include "Gap.h"

#include <dtk/ui/DrawUtil.h>
#include <dtk/core/Random.h>
#include <dtk/core/RenderUtil.h>

#include <opentimelineio/gap.h>

namespace toucan
{
    void GapWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Gap>& gap,
        const std::shared_ptr<IWidget>& parent)
    {
        IItemWidget::_init(
            context,
            app,
            gap,
            gap->getRange(),
            "toucan::ClipItem",
            parent);

        _gap = gap;
        _text = gap->getName();
        if (_text.empty())
        {
            _text = "Gap";
        }
        _color = dtk::Color4F(.3F, .3F, .3F);

        setTooltip(_text);
    }
    
    GapWidget::~GapWidget()
    {}

    std::shared_ptr<GapWidget> GapWidget::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<Gap>& gap,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<GapWidget>();
        out->_init(context, app, gap, parent);
        return out;
    }

    void GapWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItemWidget::sizeHintEvent(event);
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

    void GapWidget::clipEvent(const dtk::Box2I& clipRect, bool clipped)
    {
        IItemWidget::clipEvent(clipRect, clipped);
        if (clipped)
        {
            _draw.glyphs.clear();
        }
    }

    void GapWidget::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItemWidget::drawEvent(drawRect, event);
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
