// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "IContainer.h"

namespace toucan
{
    IContainer::IContainer()
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
}
