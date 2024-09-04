// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "TimeUnitsModel.h"

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
}
