// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "WindowModel.h"

namespace toucan
{
    WindowModel::WindowModel()
    {
        std::map<WindowControl, bool> values =
        {
            { WindowControl::ToolBar, true },
            { WindowControl::PlaybackBar, true },
            { WindowControl::TimelineWidget, true },
            { WindowControl::Tools, true }
        };
        _controls = dtk::ObservableMap<WindowControl, bool>::create(values);
        _tooltips = dtk::ObservableValue<bool>::create(true);
    }

    WindowModel::~WindowModel()
    {}

    const std::map<WindowControl, bool> WindowModel::getControls() const
    {
        return _controls->get();
    }

    std::shared_ptr<dtk::IObservableMap<WindowControl, bool> > WindowModel::observeControls() const
    {
        return _controls;
    }

    void WindowModel::setControls(const std::map<WindowControl, bool>& value)
    {
        _controls->setIfChanged(value);
    }

    bool WindowModel::getControl(WindowControl value) const
    {
        return _controls->getItem(value);
    }

    void WindowModel::setControl(WindowControl control, bool value)
    {
        _controls->setItemOnlyIfChanged(control, value);
    }

    bool WindowModel::getTooltips() const
    {
        return _tooltips->get();
    }

    std::shared_ptr<dtk::IObservableValue<bool> > WindowModel::observeTooltips() const
    {
        return _tooltips;
    }

    void WindowModel::setTooltips(bool value)
    {
        _tooltips->setIfChanged(value);
    }
}
