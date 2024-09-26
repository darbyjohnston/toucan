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

    const std::string& IItem::getName() const
    {
        return _name;
    }

    void IItem::setName(const std::string& name)
    {
        _name = name;
    }

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
