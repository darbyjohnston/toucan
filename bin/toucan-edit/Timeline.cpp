// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Timeline.h"

#include "Stack.h"

namespace toucan
{
    Timeline::Timeline(const std::filesystem::path& path) :
        _path(path)
    {
        _range = dtk::ObservableValue<OTIO_NS::TimeRange>::create();
        _stack = std::make_shared<Stack>();
        _itemRanges = dtk::ObservableList<std::shared_ptr<IItem> >::create();
    }

    Timeline::~Timeline()
    {}

    const std::filesystem::path& Timeline::getPath() const
    {
        return _path;
    }

    const OTIO_NS::TimeRange& Timeline::getRange() const
    {
        return _range->get();
    }

    std::shared_ptr<dtk::IObservableValue<OTIO_NS::TimeRange> > Timeline::observeRange() const
    {
        return _range;
    }

    void Timeline::setRange(const OTIO_NS::TimeRange& range)
    {
        _range->setIfChanged(range);
    }

    const std::shared_ptr<Stack>& Timeline::getStack() const
    {
        return _stack;
    }

    void Timeline::setItemDuration(const std::shared_ptr<IItem>& item, const OTIO_NS::RationalTime& duration)
    {
        if (auto parent = item->getParent().lock())
        {
            std::vector<std::shared_ptr<IItem> > items;

            auto children = parent->getChildren();
            auto i = std::find(children.begin(), children.end(), item);
            if (i != children.end())
            {
                OTIO_NS::TimeRange range = (*i)->getRange();
                OTIO_NS::RationalTime diff = range.duration() - duration;
                range = OTIO_NS::TimeRange(range.start_time(), duration);
                (*i)->setRange(range);
                items.push_back(*i);
                ++i;
                for (; i != children.end(); ++i)
                {
                    range = (*i)->getRange();
                    range = OTIO_NS::TimeRange(range.start_time() + diff, range.duration());
                    (*i)->setRange(range);
                    items.push_back(*i);
                }
            }

            OTIO_NS::TimeRange range;
            i = children.begin();
            if (i != children.end())
            {
                range = (*i)->getRange();
                ++i;
                for (; i != children.end(); ++i)
                {
                    range = OTIO_NS::TimeRange(
                        range.start_time(),
                        range.duration() + (*i)->getRange().duration());
                }
            }
            if (range != parent->getRange())
            {
                parent->setRange(range);
                items.push_back(parent);
            }

            range = OTIO_NS::TimeRange();
            children = _stack->getChildren();
            i = children.begin();
            if (i != children.end())
            {
                range = (*i)->getRange();
                ++i;
                for (; i != children.end(); ++i)
                {
                    range = OTIO_NS::TimeRange(
                        range.start_time(),
                        range.duration() + (*i)->getRange().duration());
                }
            }
            if (range != _stack->getRange())
            {
                _stack->setRange(range);
                items.push_back(_stack);
            }

            _range->setIfChanged(range);
            _itemRanges->setAlways(items);
        }
    }

    std::shared_ptr<dtk::IObservableList<std::shared_ptr<IItem> > > Timeline::observeItemRanges() const
    {
        return _itemRanges;
    }
}
