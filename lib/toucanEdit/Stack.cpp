// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Stack.h"

#include <algorithm>

namespace toucan
{
    Stack::Stack(
        const std::string& name,
        const OTIO_NS::TimeRange& range,
        const OTIO_NS::AnyDictionary& metadata) :
        IContainer(name, range, metadata)
    {}

    Stack::~Stack()
    {}

    OTIO_NS::TimeRange Stack::getChildRange(const std::shared_ptr<IItem>& item)
    {
        OTIO_NS::TimeRange out;
        const auto& children = getChildren();
        auto i = std::find(children.begin(), children.end(), item);
        if (i != children.end())
        {
            out = OTIO_NS::TimeRange(
                getRange().start_time(),
                (*i)->getRange().duration());
        }
        return out;
    }
}
