// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "IContainer.h"

namespace toucan
{
    IContainer::IContainer(
        const std::string& name,
        const OTIO_NS::TimeRange& range,
        const OTIO_NS::AnyDictionary& metadata) :
        IItem(name, range, metadata)
    {}

    IContainer::~IContainer()
    {}

    const std::vector<std::shared_ptr<IItem> >& IContainer::getChildren() const
    {
        return _children;
    }

    void IContainer::addChild(const std::shared_ptr<IItem>& child)
    {
        if (auto prev = child->_parent.lock())
        {
            const auto i = std::find(prev->_children.begin(), prev->_children.end(), child);
            if (i != prev->_children.end())
            {
                prev->_children.erase(i);
            }
        }
        child->_parent = std::dynamic_pointer_cast<IContainer>(shared_from_this());
        _children.push_back(child);
    }

    void IContainer::removeChild(const std::shared_ptr<IItem>& child)
    {
        const auto i = std::find(_children.begin(), _children.end(), child);
        if (i != _children.end())
        {
            child->_parent.reset();
            _children.erase(i);
        }
    }

    OTIO_NS::TimeRange IContainer::getChildRange(const std::shared_ptr<IItem>& item)
    {
        const OTIO_NS::TimeRange& range = getRange();
        OTIO_NS::RationalTime t = range.start_time();
        const auto& children = getChildren();
        auto i = children.begin();
        for (; i != children.end() && *i != item; ++i)
        {
            t += (*i)->getRange().duration();
        }
        OTIO_NS::RationalTime d(0.0, t.rate());
        if (i != children.end())
        {
            d = (*i)->getRange().duration();
        }
        return OTIO_NS::TimeRange(t, d);
    }
}
