// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <toucanEdit/IItem.h>

#include <dtk/ui/IWidget.h>

namespace toucan
{
    class App;

    class IItemWidget : public dtk::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::Context>&,
            const std::shared_ptr<App>&,
            const std::shared_ptr<IItem>&,
            const OTIO_NS::TimeRange&,
            const std::string&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~IItemWidget() = 0;

        const std::shared_ptr<IItem>& getItem() const;

        const OTIO_NS::TimeRange& getTimeRange();

        void setScale(double);
        
        bool isSelected() const;
        void setSelected(bool);

    protected:
        std::shared_ptr<IItem> _item;
        OTIO_NS::TimeRange _timeRange;
        double _scale = 100.0;
        bool _selected = false;
    };
}
