// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <dtk/core/ObservableValue.h>

#include <opentimelineio/timeline.h>

namespace toucan
{
    enum class TimeUnits
    {
        Timecode,
        Frames,
        Seconds
    };

    class TimeUnitsModel : public std::enable_shared_from_this<TimeUnitsModel>
    {
    public:
        TimeUnitsModel();

        virtual ~TimeUnitsModel();

        TimeUnits getTimeUnits() const;
        std::shared_ptr<dtk::IObservableValue<TimeUnits> > observeTimeUnits() const;
        void setTimeUnits(TimeUnits);

        OTIO_NS::RationalTime getTime(const std::string&, double rate) const;
        std::string getLabel(const OTIO_NS::RationalTime&) const;

    private:
        std::shared_ptr<dtk::ObservableValue<TimeUnits> > _timeUnits;
    };
}
