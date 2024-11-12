// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimeUnitsModel.h"

#include <sstream>

namespace toucan
{
    TimeUnitsModel::TimeUnitsModel()
    {
        _timeUnits = dtk::ObservableValue<TimeUnits>::create(TimeUnits::Timecode);
    }

    TimeUnitsModel::~TimeUnitsModel()
    {}

    TimeUnits TimeUnitsModel::getTimeUnits() const
    {
        return _timeUnits->get();
    }

    std::shared_ptr<dtk::IObservableValue<TimeUnits> > TimeUnitsModel::observeTimeUnits() const
    {
        return _timeUnits;
    }

    void TimeUnitsModel::setTimeUnits(TimeUnits value)
    {
        _timeUnits->setIfChanged(value);
    }

    OTIO_NS::RationalTime TimeUnitsModel::getTime(const std::string& text, double rate) const
    {
        OTIO_NS::RationalTime out;
        switch (_timeUnits->get())
        {
        case TimeUnits::Timecode:
            out = OTIO_NS::RationalTime::from_timecode(text, rate);
            break;
        case TimeUnits::Frames:
            out = OTIO_NS::RationalTime::from_frames(std::atof(text.c_str()), rate);
            break;
        case TimeUnits::Seconds:
            out = OTIO_NS::RationalTime::from_seconds(std::atof(text.c_str()), rate);
            break;
        default: break;
        }
        return out;
    }

    std::string TimeUnitsModel::getLabel(const OTIO_NS::RationalTime& time) const
    {
        std::string out;
        switch (_timeUnits->get())
        {
        case TimeUnits::Timecode:
            out = time.to_timecode();
            break;
        case TimeUnits::Frames:
        {
            std::stringstream ss;
            ss << time.to_frames();
            out = ss.str();
            break;
        }
        case TimeUnits::Seconds:
        {
            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << time.to_seconds();
            out = ss.str();
            break;
        }
        default: break;
        }
        return out;
    }
}
