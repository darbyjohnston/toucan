// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/core/Context.h>
#include <dtk/core/ObservableValue.h>

#include <opentimelineio/version.h>

namespace toucan
{
    //! Time units.
    enum class TimeUnits
    {
        Timecode,
        Frames,
        Seconds,

        Count,
        First = Timecode
    };
    DTK_ENUM(TimeUnits);

    //! Convert a time to a string.
    std::string toString(const OTIO_NS::RationalTime&, TimeUnits);

    //! Convert a string to a time.
    OTIO_NS::RationalTime fromString(const std::string&, TimeUnits, double rate);

    //! Time units model. 
    class TimeUnitsModel : public std::enable_shared_from_this<TimeUnitsModel>
    {
    public:
        TimeUnitsModel(const std::shared_ptr<dtk::Context>&);

        virtual ~TimeUnitsModel();

        //! Get the time units.
        TimeUnits getTimeUnits() const;

        //! Observe the time units.
        std::shared_ptr<dtk::IObservableValue<TimeUnits> > observeTimeUnits() const;

        //! Set the time units.
        void setTimeUnits(TimeUnits);

    private:
        std::weak_ptr<dtk::Context> _context;
        std::shared_ptr<dtk::ObservableValue<TimeUnits> > _timeUnits;
    };
}
