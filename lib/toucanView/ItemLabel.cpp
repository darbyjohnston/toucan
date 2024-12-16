// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ItemLabel.h"

namespace toucan
{
    void ItemLabel::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, "toucan::ItemLabel", parent);
    }

    ItemLabel::~ItemLabel()
    {}

    std::shared_ptr<ItemLabel> ItemLabel::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<ItemLabel>(new ItemLabel);
        out->_init(context, parent);
        return out;
    }

    void ItemLabel::setName(const std::string& value)
    {
        if (value == _name)
            return;
        _name = value;
        _setSizeUpdate();
        _setDrawUpdate();
    }

    void ItemLabel::setDuration(const std::string& value)
    {
        if (value == _name)
            return;
        _duration = value;
        _setSizeUpdate();
        _setDrawUpdate();
    }

    void ItemLabel::setGeometry(const dtk::Box2I& value)
    {
        IWidget::setGeometry(value);
    }

    void ItemLabel::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IWidget::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.margin = event.style->getSizeRole(dtk::SizeRole::MarginSmall, event.displayScale);
            _size.fontInfo = event.style->getFontRole(dtk::FontRole::Label, event.displayScale);
            _size.fontMetrics = event.fontSystem->getMetrics(_size.fontInfo);
            _size.nameSize = event.fontSystem->getSize(_name, _size.fontInfo);
            _size.durationSize = event.fontSystem->getSize(_duration, _size.fontInfo);
            _draw.nameGlyphs.clear();
            _draw.durationGlyphs.clear();
        }
        dtk::Size2I sizeHint;
        sizeHint.w = _size.nameSize.w + _size.margin * 2 +
            _size.durationSize.w + _size.margin * 2;
        sizeHint.h = std::max(_size.nameSize.h, _size.durationSize.h) + _size.margin * 2;
        _setSizeHint(sizeHint);
    }

    void ItemLabel::clipEvent(const dtk::Box2I& clipRect, bool clipped)
    {
        IWidget::clipEvent(clipRect, clipped);
        if (clipped)
        {
            _draw.nameGlyphs.clear();
            _draw.durationGlyphs.clear();
        }
    }

    void ItemLabel::drawEvent(const dtk::Box2I& drawRect, const dtk::DrawEvent& event)
    {
        IWidget::drawEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();

        int w = _size.nameSize.w + _size.margin * 2;
        int h = _size.fontMetrics.lineHeight + _size.margin * 2;
        const dtk::Box2I g2(
            g.min.x,
            g.min.y + g.h() / 2 - h / 2,
            w,
            h);
        if (!_name.empty() && _draw.nameGlyphs.empty())
        {
            _draw.nameGlyphs = event.fontSystem->getGlyphs(_name, _size.fontInfo);
        }
        event.render->drawText(
            _draw.nameGlyphs,
            _size.fontMetrics,
            g2.min + _size.margin,
            event.style->getColorRole(dtk::ColorRole::Text));

        w = _size.durationSize.w + _size.margin * 2;
        const dtk::Box2I g3(
            g.max.x - w,
            g.min.y + g.h() / 2 - h / 2,
            w,
            h);
        if (!dtk::intersects(g2, g3))
        {
            if (!_duration.empty() && _draw.durationGlyphs.empty())
            {
                _draw.durationGlyphs = event.fontSystem->getGlyphs(_duration, _size.fontInfo);
            }
            event.render->drawText(
                _draw.durationGlyphs,
                _size.fontMetrics,
                g3.min + _size.margin,
                event.style->getColorRole(dtk::ColorRole::Text));
        }
    }
}
