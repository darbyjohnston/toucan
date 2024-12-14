// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanView/TimeLayout.h>
#include <toucanView/TimeUnitsModel.h>

#include <dtk/ui/IWidget.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    class App;

    //! Base class for timeline items.
    class IItem : public ITimeWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>&,
            const OTIO_NS::TimeRange&,
            const std::string& objectName,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~IItem() = 0;

        //! Get the OTIO item.
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item>& getItem() const;
        
        //! Get whether the item is selected.
        bool isSelected() const;

        //! Set whether the item is selected.
        void setSelected(bool);

    protected:
        virtual void _timeUnitsUpdate();

        OTIO_NS::SerializableObject::Retainer<OTIO_NS::Item> _item;
        TimeUnits _timeUnits = TimeUnits::First;
        bool _selected = false;

    private:
        std::shared_ptr<dtk::ValueObserver<TimeUnits> > _timeUnitsObserver;
    };
}
