// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/core/ObservableList.h>
#include <dtk/core/ObservableValue.h>

#include <opentimelineio/anyDictionary.h>

namespace toucan
{
    class IItem;
    class Stack;

    class Timeline : std::enable_shared_from_this<Timeline>
    {
    public:
        Timeline();

        ~Timeline();

        const OTIO_NS::TimeRange& getRange() const;
        std::shared_ptr<dtk::IObservableValue<OTIO_NS::TimeRange> > observeRange() const;
        void setRange(const OTIO_NS::TimeRange&);
        
        const std::shared_ptr<Stack>& getStack() const;

        void setItemDuration(const std::shared_ptr<IItem>&, const OTIO_NS::RationalTime&);
        std::shared_ptr<dtk::IObservableList<std::shared_ptr<IItem> > > observeItemRanges() const;

    private:
        std::shared_ptr<dtk::ObservableValue<OTIO_NS::TimeRange> > _range;
        std::shared_ptr<Stack> _stack;
        std::shared_ptr<dtk::ObservableList<std::shared_ptr<IItem> > > _itemRanges;
    };
}

