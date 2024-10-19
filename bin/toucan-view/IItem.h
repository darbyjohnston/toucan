// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/IWidget.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    class App;

    class IItem : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&,
            const OTIO_NS::TimeRange&,
            const std::string&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~IItem() = 0;

        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& getItem() const;

        const OTIO_NS::TimeRange& getTimeRange();

        void setScale(double);
        
        bool isSelected() const;
        void setSelected(bool);

        OTIO_NS::RationalTime posToTime(double) const;
        int timeToPos(const OTIO_NS::RationalTime&) const;

    protected:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> _item;
        OTIO_NS::TimeRange _timeRange;
        double _scale = 100.0;
        bool _selected = false;
    };
}
