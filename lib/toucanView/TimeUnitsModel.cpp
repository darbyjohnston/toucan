// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "TimeUnitsModel.h"

#include <nlohmann/json.hpp>

#include <array>
#include <sstream>

namespace toucan
{
    namespace
    {
        const std::array<std::string, 3> timeUnits =
        {
            "Timecode",
            "Frames",
            "Seconds"
        };
    }

    std::string toString(TimeUnits value)
    {
        return timeUnits[static_cast<size_t>(value)];
    }

    TimeUnits fromString(const std::string& value)
    {
        const auto i = std::find(timeUnits.begin(), timeUnits.end(), value);
        return i != timeUnits.end() ?
            static_cast<TimeUnits>(i - timeUnits.begin()) :
            TimeUnits::Timecode;
    }

    TimeUnitsModel::TimeUnitsModel(const std::shared_ptr<dtk::Settings>& settings)
    {
        _settings = settings;
        TimeUnits value = TimeUnits::Timecode;
        try
        {
            const auto json = std::any_cast<nlohmann::json>(_settings->get("TimeUnits"));
            if (json.is_object())
            {
                value = toucan::fromString(json["Units"]);
            }
        }
        catch (const std::exception&)
        {}

        _timeUnits = dtk::ObservableValue<TimeUnits>::create(value);
    }

    TimeUnitsModel::~TimeUnitsModel()
    {
        nlohmann::json json;
        json["Units"] = toucan::toString(_timeUnits->get());
        _settings->set("TimeUnits", json);
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

    std::string TimeUnitsModel::toString(const OTIO_NS::RationalTime& time) const
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

    OTIO_NS::RationalTime TimeUnitsModel::fromString(const std::string& text, double rate) const
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
}
