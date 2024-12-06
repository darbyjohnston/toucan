// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/IWidget.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    class App;

    //! Base class for timeline items.
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

        //! Get the OTIO item.
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& getItem() const;

        //! Get the time range.
        const OTIO_NS::TimeRange& getTimeRange();

        //! Set the item scale.
        void setScale(double);
        
        //! Get whether the item is selected.
        bool isSelected() const;

        //! Set whether the item is selected.
        void setSelected(bool);

        //! Convert a position to a time.
        OTIO_NS::RationalTime posToTime(double) const;

        //! Convert a time to a position.
        int timeToPos(const OTIO_NS::RationalTime&) const;

    protected:
        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> _item;
        OTIO_NS::TimeRange _timeRange;
        double _scale = 100.0;
        bool _selected = false;
    };
}
