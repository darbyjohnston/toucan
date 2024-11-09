// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "StackItem.h"

#include "TrackItem.h"

#include <dtk/ui/DrawUtil.h>
#include <dtk/core/RenderUtil.h>

namespace toucan
{
    void StackItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Stack>& stack,
        const std::shared_ptr<IWidget>& parent)
    {
        IItem::_init(
            context,
            app,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(stack),
            stack->trimmed_range(),
            "toucan::StackItem",
            parent);

        _stack = stack;
        _text = !stack->name().empty() ? stack->name() : "Stack";
        _color = dtk::Color4F(.2F, .2F, .2F);

        setTooltip(_text);

        for (const auto& child : stack->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(child))
            {
                TrackItem::create(context, app, track, shared_from_this());
            }
        }
    }

    StackItem::~StackItem()
    {}

    std::shared_ptr<StackItem> StackItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Stack>& stack,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<StackItem>();
        out->_init(context, app, stack, parent);
        return out;
    }

    void StackItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        const dtk::Box2I& g = getGeometry();
        dtk::V2I pos = g.min;
        pos.y += _size.fontMetrics.lineHeight + _size.margin * 2 + _size.border * 2;
        for (const auto& child : getChildren())
        {
            const dtk::Size2I& sizeHint = child->getSizeHint();
            child->setGeometry(dtk::Box2I(pos.x, pos.y, sizeHint.w, sizeHint.h));
            pos.y += sizeHint.h;
        }
    }

    void StackItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
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
        const auto& children = getChildren();
        for (const auto& child : children)
        {
            const dtk::Size2I& childSizeHint = child->getSizeHint();
            sizeHint.h += childSizeHint.h;
        }
        sizeHint.h += _size.textSize.h + _size.margin * 2 + _size.border * 4;
        _setSizeHint(sizeHint);
    }

    void StackItem::clipEvent(const dtk::Box2I& clipRect, bool clipped)
    {
        IItem::clipEvent(clipRect, clipped);
        if (clipped)
        {
            _draw.glyphs.clear();
        }
    }

    void StackItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
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
