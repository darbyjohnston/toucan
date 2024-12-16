// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "GapItem.h"

#include <dtk/ui/DrawUtil.h>

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
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::SerializableObjectWithMetadata>(gap),
            opt.has_value() ? opt.value() : OTIO_NS::TimeRange(),
            "toucan::ClipItem",
            parent);

        _gap = gap;
        _text = !gap->name().empty() ? gap->name() : "Gap";
        _color = dtk::Color4F(.3F, .3F, .3F);

        setTooltip(gap->schema_name() + ": " + _text);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _label = ItemLabel::create(context, _layout);
        _label->setName(_text);

        _textUpdate();
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

    void GapItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        _layout->setGeometry(value);
    }

    void GapItem::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        IItem::sizeHintEvent(event);
        const bool displayScaleChanged = event.displayScale != _size.displayScale;
        if (_size.init || displayScaleChanged)
        {
            _size.init = false;
            _size.displayScale = event.displayScale;
            _size.border = event.style->getSizeRole(dtk::SizeRole::Border, event.displayScale);
        }
        _setSizeHint(_layout->getSizeHint());
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
    }

    void GapItem::_timeUnitsUpdate()
    {
        _textUpdate();
    }

    void GapItem::_textUpdate()
    {
        if (_label)
        {
            std::string text = toString(_timeRange.duration(), _timeUnits);
            _label->setDuration(text);
        }
    }
}
