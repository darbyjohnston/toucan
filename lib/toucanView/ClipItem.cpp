// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ClipItem.h"

#include <dtk/ui/DrawUtil.h>

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
        const OTIO_NS::TimeRange timeRange = opt.has_value() ? opt.value() : OTIO_NS::TimeRange();
        IItem::_init(
            context,
            app,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::SerializableObjectWithMetadata>(clip),
            timeRange,
            "toucan::ClipItem",
            parent);

        _setMousePressEnabled(0, 0);

        _clip = clip;
        _text = !clip->name().empty() ? clip->name() : "Clip";
        _color = color;

        setTooltip(clip->schema_name() + ": " + _text);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::None);

        _label = ItemLabel::create(context, _layout);
        _label->setName(_text);

        const auto& markers = clip->markers();
        for (const auto& marker : markers)
        {
            auto markerItem = MarkerItem::create(context, app, marker, timeRange, _layout);
            _markerItems.push_back(markerItem);
        }

        _textUpdate();
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

    void ClipItem::setScale(double value)
    {
        IItem::setScale(value);
        for (const auto& markerItem : _markerItems)
        {
            markerItem->setScale(value);
        }
    }

    void ClipItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        _layout->setGeometry(value);
    }

    void ClipItem::sizeHintEvent(const dtk::SizeHintEvent& event)
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
    
    void ClipItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
        const dtk::Box2I& g = _label->getGeometry();
        const dtk::Box2I g2 = dtk::margin(g, -_size.border, 0, -_size.border, 0);
        event.render->drawRect(
            g2,
            _selected ? event.style->getColorRole(dtk::ColorRole::Yellow) : _color);
    }

    void ClipItem::_timeUnitsUpdate()
    {
        _textUpdate();
    }

    void ClipItem::_textUpdate()
    {
        if (_label)
        {
            std::string text = toString(_timeRange.duration(), _timeUnits);
            _label->setDuration(text);
        }
    }
}
