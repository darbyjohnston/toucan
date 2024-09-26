// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "IItemWidget.h"

namespace toucan
{
    void IItemWidget::_init(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<App>& app,
        const std::shared_ptr<IItem>& item,
        const OTIO_NS::TimeRange& timeRange,
        const std::string& name,
        const std::shared_ptr<IWidget>& parent)
    {
        IWidget::_init(context, name, parent);
        _item = item;
        _timeRange = timeRange;
    }

    IItemWidget::~IItemWidget()
    {}

    const std::shared_ptr<IItem>& IItemWidget::getItem() const
    {
        return _item;
    }

    const OTIO_NS::TimeRange& IItemWidget::getTimeRange()
    {
        return _timeRange;
    }

    void IItemWidget::setScale(double value)
    {
        if (value == _scale)
            return;
        _scale = value;
        for (const auto& child : getChildren())
        {
            if (auto item = std::dynamic_pointer_cast<IItemWidget>(child))
            {
                item->setScale(value);
            }
        }
        _setSizeUpdate();
    }

    bool IItemWidget::isSelected() const
    {
        return _selected;
    }

    void IItemWidget::setSelected(bool value)
    {
        if (value == _selected)
            return;
        _selected = value;
        _setDrawUpdate();
    }
}
