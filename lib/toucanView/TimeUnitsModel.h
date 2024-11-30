// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/ui/Settings.h>
#include <dtk/core/ObservableValue.h>

#include <opentimelineio/version.h>

namespace toucan
{
    //! Time units.
    enum class TimeUnits
    {
        Timecode,
        Frames,
        Seconds
    };

    //! Convert to a string.
    std::string toString(TimeUnits);

    //! Convert from a string.
    TimeUnits fromString(const std::string&);

    //! Time units model. 
    class TimeUnitsModel : public std::enable_shared_from_this<TimeUnitsModel>
    {
    public:
        TimeUnitsModel(const std::shared_ptr<dtk::Settings>&);

        virtual ~TimeUnitsModel();

        //! Get the time units.
        TimeUnits getTimeUnits() const;

        //! Observe the time units.
        std::shared_ptr<dtk::IObservableValue<TimeUnits> > observeTimeUnits() const;

        //! Set the time units.
        void setTimeUnits(TimeUnits);

        //! Convert a time to a string.
        std::string toString(const OTIO_NS::RationalTime&) const;

        //! Convert a string to a time.
        OTIO_NS::RationalTime fromString(const std::string&, double rate) const;

    private:
        std::shared_ptr<dtk::Settings> _settings;
        std::shared_ptr<dtk::ObservableValue<TimeUnits> > _timeUnits;
    };
}
