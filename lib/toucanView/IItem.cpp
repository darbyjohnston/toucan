// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "IItem.h"

#include "App.h"

namespace toucan
{
    void IItem::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>& object,
        const OTIO_NS::TimeRange& timeRange,
        const std::string& objectName,
        const std::shared_ptr<IWidget>& parent)
    {
        ITimeWidget::_init(context, timeRange, objectName, parent);

        _object = object;

        _timeUnitsObserver = dtk::ValueObserver<TimeUnits>::create(
            app->getTimeUnitsModel()->observeTimeUnits(),
            [this](TimeUnits value)
            {
                _timeUnits = value;
                _timeUnitsUpdate();
            });
    }

    IItem::~IItem()
    {}

    const OTIO_NS::SerializableObject::Retainer<OTIO_NS::SerializableObjectWithMetadata>& IItem::getObject() const
    {
        return _object;
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

    void IItem::_timeUnitsUpdate()
    {}
}
