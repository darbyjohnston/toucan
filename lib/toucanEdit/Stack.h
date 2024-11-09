// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanEdit/IContainer.h>

namespace toucan
{
    class Stack : public IContainer
    {
    public:
        Stack(
            const std::string& name = std::string(),
            const OTIO_NS::TimeRange& range = OTIO_NS::TimeRange(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary());

        virtual ~Stack();

        OTIO_NS::TimeRange getChildRange(const std::shared_ptr<IItem>&) override;
    };
}

