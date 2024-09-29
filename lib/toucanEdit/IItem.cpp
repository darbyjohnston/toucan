// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "IItem.h"

#include "IContainer.h"

namespace toucan
{
    IItem::IItem(
        const std::string& name,
        const OTIO_NS::TimeRange& range,
        const OTIO_NS::AnyDictionary& metadata) :
        _name(name),
        _range(range),
        _metadata(metadata)
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

    const OTIO_NS::AnyDictionary& IItem::getMetadata() const
    {
        return _metadata;
    }

    void IItem::setMetadata(const OTIO_NS::AnyDictionary& value)
    {
        _metadata = value;
    }

    std::shared_ptr<IContainer> IItem::getParent() const
    {
        return _parent.lock();
    }

    std::shared_ptr<IContainer> IItem::getRoot()
    {
        std::shared_ptr<IItem> item = shared_from_this();
        for (; item->getParent(); item = item->getParent())
            ;
        return std::dynamic_pointer_cast<IContainer>(item);
    }

    const std::vector<std::shared_ptr<Effect> >& IItem::getEffects() const
    {
        return _effects;
    }

    void IItem::setEffects(const std::vector<std::shared_ptr<Effect> >& value)
    {
        _effects = value;
    }

    OTIO_NS::RationalTime IItem::transform(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<IItem>& toItem)
    {
        OTIO_NS::RationalTime out = time;

        auto root = getRoot();
        auto item = shared_from_this();
        while (item != toItem && item != root)
        {
            out -= item->getRange().start_time();
            auto parent = item->getParent();
            out += parent->getChildRange(item).start_time();
            item = parent;
        }

        auto ancestor = item;
        item = toItem;
        while (item != ancestor && item != root)
        {
            out += item->getRange().start_time();
            auto parent = item->getParent();
            out -= parent->getChildRange(item).start_time();
            item = parent;
        }

        return out;
    }

    OTIO_NS::TimeRange IItem::transform(
        const OTIO_NS::TimeRange& range,
        const std::shared_ptr<IItem>& toItem)
    {
        return OTIO_NS::TimeRange(
            transform(range.start_time(), toItem),
            range.duration());
    }
}
