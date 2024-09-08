// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

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
}
