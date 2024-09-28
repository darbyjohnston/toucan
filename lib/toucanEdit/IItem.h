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

        std::shared_ptr<IContainer> getParent() const;

        std::shared_ptr<IContainer> getRoot();

        OTIO_NS::RationalTime transform(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<IItem>&);

        OTIO_NS::TimeRange transform(
            const OTIO_NS::TimeRange&,
            const std::shared_ptr<IItem>&);

    private:
        std::string _name;
        OTIO_NS::TimeRange _range;
        std::weak_ptr<IContainer> _parent;

        friend class IContainer;
    };
}

