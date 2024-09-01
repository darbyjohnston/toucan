// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#pragma once

#include <dtkUI/IWidget.h>

namespace toucan
{
    class GraphWidget : public dtk::ui::IWidget
    {
    protected:
        void _init(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent);

    public:
        virtual ~GraphWidget();

        static std::shared_ptr<GraphWidget> create(
            const std::shared_ptr<dtk::core::Context>&,
            const std::shared_ptr<IWidget>& parent = nullptr);

    private:
    };
}

