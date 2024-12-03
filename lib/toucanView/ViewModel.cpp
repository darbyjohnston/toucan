// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ViewModel.h"

#include <sstream>

namespace toucan
{
    ViewModel::ViewModel()
    {
        _zoomIn = dtk::ObservableValue<bool>::create(false);
        _zoomOut = dtk::ObservableValue<bool>::create(false);
        _zoomReset = dtk::ObservableValue<bool>::create(false);
        _frameView = dtk::ObservableValue<bool>::create(true);
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

    bool ViewModel::getFrameView() const
    {
        return _frameView->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > ViewModel::observeFrameView() const
    {
        return _frameView;
    }

    void ViewModel::setFrameView(bool value)
    {
        _frameView->setIfChanged(value);
    }
}
