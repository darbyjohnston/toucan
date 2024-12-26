// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimeUnitsModel.h"

#include <dtk/ui/Settings.h>
#include <dtk/core/Error.h>
#include <dtk/core/Format.h>
#include <dtk/core/String.h>

#include <nlohmann/json.hpp>

#include <array>
#include <sstream>

namespace toucan
{
    DTK_ENUM_IMPL(
        TimeUnits,
        "Timecode",
        "Frames",
        "Seconds");

    std::string toString(const OTIO_NS::RationalTime& time, TimeUnits units)
    {
        std::string out;
        switch (units)
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

    OTIO_NS::RationalTime fromString(const std::string& text, TimeUnits units, double rate)
    {
        OTIO_NS::RationalTime out;
        switch (units)
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

    std::string toString(const OTIO_NS::TimeRange& timeRange, TimeUnits units)
    {
        return dtk::Format("{0} - {1} : {2} @ {3}").
            arg(toString(timeRange.start_time(), units)).
            arg(toString(timeRange.end_time_inclusive(), units)).
            arg(toString(timeRange.duration(), units)).
            arg(timeRange.duration().rate());
    }

    TimeUnitsModel::TimeUnitsModel(const std::shared_ptr<dtk::Context>& context)
    {
        _context = context;

        TimeUnits value = TimeUnits::Timecode;
        try
        {
            auto settings = context->getSystem<dtk::Settings>();
            const auto json = std::any_cast<nlohmann::json>(settings->get("TimeUnits"));
            auto i = json.find("Units");
            if (i != json.end() && i->is_string())
            {
                std::stringstream ss(i->get<std::string>());
                ss >> value;
            }
        }
        catch (const std::exception&)
        {}

        _timeUnits = dtk::ObservableValue<TimeUnits>::create(value);
    }

    TimeUnitsModel::~TimeUnitsModel()
    {
        nlohmann::json json;
        std::stringstream ss;
        ss << _timeUnits->get();
        json["Units"] = ss.str();
        auto context = _context.lock();
        auto settings = context->getSystem<dtk::Settings>();
        settings->set("TimeUnits", json);
    }

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
}
