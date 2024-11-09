// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "IItem.h"

namespace toucan
{
    void IItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& item,
        const OTIO_NS::TimeRange& timeRange,
        const std::string& name,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, name, parent);
        _item = item;
        _timeRange = timeRange;
    }

    IItem::~IItem()
    {}

    const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& IItem::getItem() const
    {
        return _item;
    }

    const OTIO_NS::TimeRange& IItem::getTimeRange()
    {
        return _timeRange;
    }

    void IItem::setScale(double value)
    {
        if (value == _scale)
            return;
        _scale = value;
        for (const auto& child : getChildren())
        {
            if (auto item = std::dynamic_pointer_cast<IItem>(child))
            {
                item->setScale(value);
            }
        }
        _setSizeUpdate();
    }

    bool IItem::isSelected() const
    {
        return _selected;
    }

    void IItem::setSelected(bool value)
    {
        if (value == _selected)
            return;
        _selected = value;
        _setDrawUpdate();
    }

    OTIO_NS::RationalTime IItem::posToTime(double value) const
    {
        OTIO_NS::RationalTime out;
        const dtk::Box2I& g = getGeometry();
        if (g.w() > 0)
        {
            const double normalized = (value - g.min.x) /
                static_cast<double>(_timeRange.duration().rescaled_to(1.0).value() * _scale);
            out = OTIO_NS::RationalTime(
                _timeRange.start_time() +
                OTIO_NS::RationalTime(
                    _timeRange.duration().value() * normalized,
                    _timeRange.duration().rate())).
                round();
            out = dtk::clamp(
                out,
                _timeRange.start_time(),
                _timeRange.end_time_inclusive());
        }
        return out;
    }

    int IItem::timeToPos(const OTIO_NS::RationalTime& value) const
    {
        const dtk::Box2I& g = getGeometry();
        const OTIO_NS::RationalTime t = value - _timeRange.start_time();
        return g.min.x + t.rescaled_to(1.0).value() * _scale;
    }
}
