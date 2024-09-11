// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/core/ObservableMap.h>
#include <dtk/core/ObservableValue.h>

namespace toucan
{
    enum class WindowControl
    {
        ToolBar,
        BottomBar,
        TimelineWidget,
        Tools
    };

    class WindowModel : public std::enable_shared_from_this<WindowModel>
    {
    public:
        WindowModel();

        virtual ~WindowModel();

        const std::map<WindowControl, bool> getControls() const;
        std::shared_ptr<dtk::IObservableMap<WindowControl, bool> > observeControls() const;
        void setControls(const std::map<WindowControl, bool>&);

        bool getControl(WindowControl) const;
        void setControl(WindowControl, bool);

        bool getTooltips() const;
        std::shared_ptr<dtk::IObservableValue<bool> > observeTooltips() const;
        void setTooltips(bool);

    private:
        std::shared_ptr<dtk::ObservableMap<WindowControl, bool> > _controls;
        std::shared_ptr<dtk::ObservableValue<bool> > _tooltips;
    };
}
