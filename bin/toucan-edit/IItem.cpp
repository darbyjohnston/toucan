// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "IItem.h"

#include "IContainer.h"

namespace toucan
{
    IItem::IItem()
    {}

    IItem::~IItem()
    {}

    const OTIO_NS::TimeRange& IItem::getRange() const
    {
        return _range;
    }

    void IItem::setRange(const OTIO_NS::TimeRange& range)
    {
        _range = range;
    }

    const std::weak_ptr<IContainer>& IItem::getParent() const
    {
        return _parent;
    }
}
