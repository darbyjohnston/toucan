// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <opentimelineio/anyDictionary.h>

#include <memory>

namespace toucan
{
    class IContainer;

    class IItem : public std::enable_shared_from_this<IItem>
    {
    public:
        IItem();

        virtual ~IItem() = 0;

        const std::string& getName() const;
        void setName(const std::string&);

        const OTIO_NS::TimeRange& getRange() const;
        void setRange(const OTIO_NS::TimeRange&);

        const std::weak_ptr<IContainer>& getParent() const;

    private:
        std::string _name;
        OTIO_NS::TimeRange _range;
        std::weak_ptr<IContainer> _parent;

        friend class IContainer;
    };
}

