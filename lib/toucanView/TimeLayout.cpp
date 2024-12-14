// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimeLayout.h"

namespace toucan
{
    void ITimeWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::TimeRange& timeRange,
        const std::string& objectName,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        dtk::IWidget::_init(context, objectName, parent);
        _timeRange = timeRange;
    }

    ITimeWidget::~ITimeWidget()
    {}

    const OTIO_NS::TimeRange& ITimeWidget::getTimeRange()
    {
        return _timeRange;
    }

    double ITimeWidget::getScale() const
    {
        return _scale;
    }

    void ITimeWidget::setScale(double value)
    {
        if (value == _scale)
            return;
        _scale = value;
        for (const auto& child : getChildren())
        {
            if (auto item = std::dynamic_pointer_cast<ITimeWidget>(child))
            {
                item->setScale(value);
            }
        }
        _setSizeUpdate();
    }

    OTIO_NS::RationalTime ITimeWidget::posToTime(double value) const
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

    int ITimeWidget::timeToPos(const OTIO_NS::RationalTime& value) const
    {
        const dtk::Box2I& g = getGeometry();
        const OTIO_NS::RationalTime t = value - _timeRange.start_time();
        return g.min.x + t.rescaled_to(1.0).value() * _scale;
    }

    void TimeLayout::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::TimeRange& timeRange,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        ITimeWidget::_init(context, timeRange, "toucan::TimeLayout", parent);
    }

    TimeLayout::~TimeLayout()
    {}

    std::shared_ptr<TimeLayout> TimeLayout::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::TimeRange& timeRange,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<TimeLayout>(new TimeLayout);
        out->_init(context, timeRange, parent);
        return out;
    }

    void TimeLayout::setGeometry(const dtk::Box2I& value)
    {
        ITimeWidget::setGeometry(value);
        for (const auto& child : getChildren())
        {
            if (auto timeWidget = std::dynamic_pointer_cast<ITimeWidget>(child))
            {
                const OTIO_NS::TimeRange& timeRange = timeWidget->getTimeRange();
                const double t0 = timeToPos(timeRange.start_time());
                const double t1 = timeToPos(timeRange.end_time_exclusive());
                const dtk::Size2I& childSizeHint = child->getSizeHint();
                child->setGeometry(dtk::Box2I(t0, value.min.y, t1 - t0, childSizeHint.h));
            };
        }
    }

    void TimeLayout::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        ITimeWidget::sizeHintEvent(event);
        dtk::Size2I sizeHint;
        for (const auto& child : getChildren())
        {
            if (auto timeWidget = std::dynamic_pointer_cast<ITimeWidget>(child))
            {
                const dtk::Size2I& childSizeHint = timeWidget->getSizeHint();
                sizeHint.h = std::max(sizeHint.h, childSizeHint.h);
            }
        }
        sizeHint.w = _timeRange.duration().rescaled_to(1.0).value() * _scale;
        _setSizeHint(sizeHint);
    }

    void TimeStackLayout::_init(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::TimeRange& timeRange,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        ITimeWidget::_init(context, timeRange, "toucan::TimeStackLayout", parent);
    }

    TimeStackLayout::~TimeStackLayout()
    {}

    std::shared_ptr<TimeStackLayout> TimeStackLayout::create(
        const std::shared_ptr<dtk::Context>& context,
        const OTIO_NS::TimeRange& timeRange,
        const std::shared_ptr<dtk::IWidget>& parent)
    {
        auto out = std::shared_ptr<TimeStackLayout>(new TimeStackLayout);
        out->_init(context, timeRange, parent);
        return out;
    }

    void TimeStackLayout::setGeometry(const dtk::Box2I& value)
    {
        ITimeWidget::setGeometry(value);
        int y = value.min.y;
        for (const auto& child : getChildren())
        {
            if (auto timeWidget = std::dynamic_pointer_cast<ITimeWidget>(child))
            {
                const OTIO_NS::TimeRange& timeRange = timeWidget->getTimeRange();
                const double t0 = timeToPos(timeRange.start_time());
                const double t1 = timeToPos(timeRange.end_time_exclusive());
                const dtk::Size2I& childSizeHint = child->getSizeHint();
                child->setGeometry(dtk::Box2I(t0, y, t1 - t0, childSizeHint.h));
                y += childSizeHint.h;
            };
        }
    }

    void TimeStackLayout::sizeHintEvent(const dtk::SizeHintEvent& event)
    {
        ITimeWidget::sizeHintEvent(event);
        dtk::Size2I sizeHint;
        for (const auto& child : getChildren())
        {
            if (auto timeWidget = std::dynamic_pointer_cast<ITimeWidget>(child))
            {
                const dtk::Size2I& childSizeHint = timeWidget->getSizeHint();
                sizeHint.h += childSizeHint.h;
            }
        }
        sizeHint.w = _timeRange.duration().rescaled_to(1.0).value() * _scale;
        _setSizeHint(sizeHint);
    }
}
