// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TrackItem.h"

#include "ClipItem.h"
#include "GapItem.h"

#include <dtk/ui/DrawUtil.h>
#include <dtk/core/RenderUtil.h>

namespace toucan
{
    void TrackItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        OTIO_NS::TimeRange timeRange;
        if (track->trimmed_range_in_parent().has_value())
        {
            timeRange = track->trimmed_range_in_parent().value();
        }
        IItem::_init(
            context,
            app,
            OTIO_NS::dynamic_retainer_cast<OTIO_NS::Item>(track),
            timeRange,
            "toucan::TrackItem",
            parent);

        _track = track;
        _text = !track->name().empty() ? track->name() : (track->kind() + " Track");
        _color = OTIO_NS::Track::Kind::video == track->kind() ?
            dtk::Color4F(.2F, .2F, .3F) :
            dtk::Color4F(.2F, .3F, .2F);

        setTooltip(_text);

        _layout = dtk::VerticalLayout::create(context, shared_from_this());
        _layout->setSpacingRole(dtk::SizeRole::SpacingTool);

        _label = ItemLabel::create(context, _layout);
        _label->setName(_text);

        _timeLayout = TimeLayout::create(context, timeRange, _layout);

        for (const auto& child : track->children())
        {
            if (auto clip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(child))
            {
                const dtk::Color4F color =
                    OTIO_NS::Track::Kind::video == track->kind() ?
                    dtk::Color4F(.4F, .4F, .6F) :
                    dtk::Color4F(.4F, .6F, .4F);
                ClipItem::create(context, app, clip, color, _timeLayout);
            }
            else if (auto gap = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Gap>(child))
            {
                GapItem::create( context, app, gap, _timeLayout);
            }
        }

        _textUpdate();
    }

    TrackItem::~TrackItem()
    {}

    std::shared_ptr<TrackItem> TrackItem::create(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Track>& track,
        const std::shared_ptr<IWidget>& parent)
    {
        auto out = std::make_shared<TrackItem>();
        out->_init(context, app, track, parent);
        return out;
    }

    void TrackItem::setScale(double value)
    {
        _timeLayout->setScale(value);
    }

    void TrackItem::setGeometry(const dtk::Box2I& value)
    {
        IItem::setGeometry(value);
        _layout->setGeometry(value);
    }

    void TrackItem::sizeHintEvent(const dtk::SizeHintEvent& event)
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

    void TrackItem::drawEvent(
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

    void TrackItem::_timeUnitsUpdate()
    {
        _textUpdate();
    }

    void TrackItem::_textUpdate()
    {
        if (_label)
        {
            std::string text = toString(_timeRange.duration(), _timeUnits);
            _label->setDuration(text);
        }
    }
}
