// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ViewModel.h"

#include <sstream>

namespace toucan
{
    ViewModel::ViewModel()
    {
        _zoomIn = dtk::ObservableValue<bool>::create(false);
        _zoomOut = dtk::ObservableValue<bool>::create(false);
        _zoomReset = dtk::ObservableValue<bool>::create(false);
        _frame = dtk::ObservableValue<bool>::create(true);
    }

    ViewModel::~ViewModel()
    {}

    void ViewModel::zoomIn()
    {
        _zoomIn->setAlways(true);
    }

    void ViewModel::zoomOut()
    {
        _zoomOut->setAlways(true);
    }

    void ViewModel::zoomReset()
    {
        _zoomReset->setAlways(true);
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeZoomIn() const
    {
        return _zoomIn;
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeZoomOut() const
    {
        return _zoomOut;
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeZoomReset() const
    {
        return _zoomReset;
    }

    bool ViewModel::getFrame() const
    {
        return _frame->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeFrame() const
    {
        return _frame;
    }

    void ViewModel::setFrame(bool value)
    {
        _frame->setIfChanged(value);
    }
}
