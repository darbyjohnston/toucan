// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <opentimelineio/anyDictionary.h>

#include <memory>
#include <vector>

namespace toucan
{
    class Effect;
    class IContainer;

    class IItem : public std::enable_shared_from_this<IItem>
    {
    public:
        IItem(
            const std::string& name = std::string(),
            const OTIO_NS::TimeRange& range = OTIO_NS::TimeRange(),
            const OTIO_NS::AnyDictionary& metadata = OTIO_NS::AnyDictionary());

        virtual ~IItem() = 0;

        const std::string& getName() const;
        void setName(const std::string&);

        const OTIO_NS::TimeRange& getRange() const;
        void setRange(const OTIO_NS::TimeRange&);

        const OTIO_NS::AnyDictionary& getMetadata() const;
        void setMetadata(const OTIO_NS::AnyDictionary&);

        std::shared_ptr<IContainer> getParent() const;
        std::shared_ptr<IContainer> getRoot();

        const std::vector<std::shared_ptr<Effect> >& getEffects() const;
        void setEffects(const std::vector<std::shared_ptr<Effect> >&);

        OTIO_NS::RationalTime transform(
            const OTIO_NS::RationalTime&,
            const std::shared_ptr<IItem>&);

        OTIO_NS::TimeRange transform(
            const OTIO_NS::TimeRange&,
            const std::shared_ptr<IItem>&);

    private:
        std::string _name;
        OTIO_NS::TimeRange _range;
        OTIO_NS::AnyDictionary _metadata;
        std::weak_ptr<IContainer> _parent;
        std::vector<std::shared_ptr<Effect> > _effects;

        friend class IContainer;
    };
}

