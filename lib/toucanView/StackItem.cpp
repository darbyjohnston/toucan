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
        const OTIO_NS::TimeRange timeRange = stack->trimmed_range();
        IItem::_init(
            context,
            app,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(stack),
            timeRange,
            "toucan::StackItem",
            parent);

        _stack = stack;
        _text = !stack->name().empty() ? stack->name() : "Stack";
        _color = dtk::Color4F(.2F, .2F, .2F);

        setTooltip(_text);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _label = ItemLabel::create(context, _layout);
        _label->setName(_text);

        _timeLayout = TimeStackLayout::create(context, timeRange, _layout);

        for (const auto& child : stack->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(child))
            {
                TrackItem::create(context, app, track, _timeLayout);
            }
        }

        _textUpdate();
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

    void StackItem::setScale(double value)
    {
        _timeLayout->setScale(value);
    }

    void StackItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        _layout->setGeometry(value);
    }

    void StackItem::sizeHintEvent(const dtk::SizeHintEvent& event)
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

    void StackItem::drawEvent(
        const dtk::Box2I& drawRect,
        const dtk::DrawEvent& event)
    {
        IItem::drawEvent(drawRect, event);
        const dtk::Box2I& g = getGeometry();

        const dtk::Box2I g2 = dtk::margin(g, -_size.border, 0);
        event.render->drawRect(
            g2,
            _selected ? event.style->getColorRole(dtk::ColorRole::Yellow) : _color);
    }

    void StackItem::_timeUnitsUpdate()
    {
        _textUpdate();
    }

    void StackItem::_textUpdate()
    {
        if (_label)
        {
            std::string text = toString(_timeRange.duration(), _timeUnits);
            _label->setDuration(text);
        }
    }
}
