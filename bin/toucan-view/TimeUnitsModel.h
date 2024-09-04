// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/core/ObservableValue.h>

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

    private:
        std::shared_ptr<dtk::ObservableValue<TimeUnits> > _timeUnits;
    };
}
