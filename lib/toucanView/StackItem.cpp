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
        const std::shared_ptr<File>& file,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Stack>& stack,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline ,
        const std::shared_ptr<IWidget>& parent)
    {
        OTIO_NS::TimeRange timeRange = stack->trimmed_range();
        if (timeline->global_start_time().has_value())
        {
            timeRange = OTIO_NS::TimeRange(
                timeline->global_start_time().value() + timeRange.start_time(),
                timeRange.duration());
        }
        timeRange = OTIO_NS::TimeRange(
            timeRange.start_time().round(),
            timeRange.duration().round());
        IItem::_init(
            context,
            app,
            file,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::SerializableObjectWithMetadata>(stack),
            timeRange,
            "toucan::StackItem",
            parent);

        _stack = stack;
        _text = !stack->name().empty() ? stack->name() : "Stack";
        _color = dtk::Color4F(.2F, .2F, .2F);

        setTooltip(stack->schema_name() + ": " + _text);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _label = ItemLabel::create(context, _layout);
        _label->setName(_text);

        const auto& markers = stack->markers();
        if (!markers.empty())
        {
            _markerLayout = TimeLayout::create(context, timeRange, _layout);
            for (const auto& marker : markers)
            {
                OTIO_NS::TimeRange markerTimeRange = marker->marked_range();
                if (timeline->global_start_time().has_value())
                {
                    markerTimeRange = OTIO_NS::TimeRange(
                        timeline->global_start_time().value() + markerTimeRange.start_time(),
                        markerTimeRange.duration());
                }
                auto markerItem = MarkerItem::create(
                    context,
                    app,
                    file,
                    marker,
                    markerTimeRange,
                    _markerLayout);
                _markerItems.push_back(markerItem);
            }
        }

        _timeLayout = TimeStackLayout::create(context, timeRange, _layout);
        for (const auto& child : stack->children())
        {
            if (auto track = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(child))
            {
                TrackItem::create(
                    context,
                    app,
                    file,
                    track,
                    timeline,
                    _timeLayout);
            }
        }

        _textUpdate();
    }

    StackItem::~StackItem()
    {}

    std::shared_ptr<StackItem> StackItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<File>& file,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Stack>& stack,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<StackItem>();
        out->_init(context, app, file, stack, timeline, parent);
        return out;
    }

    void StackItem::setScale(double value)
    {
        IItem::setScale(value);
        if (_markerLayout)
        {
            _markerLayout->setScale(value);
        }
        _timeLayout->setScale(value);
    }

    void StackItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        _layout->setGeometry(value);
        _geom.g2 = dtk::margin(value, -_size.border, 0, -_size.border, 0);
        _geom.g3 = dtk::margin(_label->getGeometry(), -_size.border, 0, -_size.border, 0);
        _selectionRect = _geom.g3;
    }

    dtk::Box2I StackItem::getChildrenClipRect() const
    {
        return _geom.g2;
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
        event.render->drawRect(
            _geom.g3,
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
